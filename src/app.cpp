#include "../headers/app.hpp"
#include <iostream>

// App section itself

App::~App() {
    App::ShutdownAudio();
    App::ShutdownSDL();
}

void App::Run() {
    // Initialisation
    App::InitAudio();
    App::InitSDL();

    // App/event loop

    bool running = true;
    SDL_Event event;

    // MIDI section

    // lists MIDI ports - discovery, nothing is being opened yet
    libremidi::observer obs;
    auto input_ports = obs.get_input_ports();

    if(input_ports.empty()) {
        std::cout << "No MIDI input devices found.\n";
    } else {
        std::cout << "Available MIDI input ports:\n";
        int idx = 0;
        for(const auto& port : input_ports) {
            std::cout << "  [" << idx++ << "] " << port.port_name << std::endl;
        }
    }

    // a lambda callback used for MIDI - this is passed to capture the App object,
    // so that the _synth member functions can be called later

    // bytes are decoded like this:
        // msg.bytes[0] - status (type of message) and the channel packed together
        // msg.bytes[1] - note number (0-127)
        // msg.bytes[2] - velocity (0-127)
    auto midi_callback = [this](const libremidi::message& msg)
    {
        // MIDI message data is in msg.bytes
        if(msg.size() < 3) return; // ignore non-note messages

        auto status = msg.bytes[0] & 0xf0;  // extracts the top 4 bits (status)
        auto channel = msg.bytes[0] & 0x0f; // extracts the bottom 4 bits (channel)
        auto note = msg.bytes[1];
        auto velocity = msg.bytes[2];

        if(status == 0x90 && velocity > 0) { // Note On
            std::cout << "[MIDI] Note ON: " << static_cast<int>(note) << ", vel: " << static_cast<int>(velocity) << std::endl;
            std::lock_guard<std::mutex> lock(_synthMutex);
            this->_synth.NoteOn(note, velocity);
        }
        else if((status == 0x80) || (status == 0x90 && velocity == 0)) { // Note Off
            std::cout << "[MIDI] Note OFF: " << static_cast<int>(note) << std::endl;
            std::lock_guard<std::mutex> lock(_synthMutex);
            this->_synth.NoteOff(note);
        }
    };

    libremidi::input_configuration config;
    config.on_message = midi_callback;

    libremidi::midi_in midi_in{config};

    // open the first available device
    if (!input_ports.empty()) {
        midi_in.open_port(input_ports[0]);
        std::cout << "Opened MIDI input: " << input_ports[0].port_name << std::endl;
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                App::HandleKeyDown(event.key.keysym.sym);
            }
            if (event.type == SDL_KEYUP) {
                App::HandleKeyUp(event.key.keysym.sym);
            }

            SDL_Delay(5);
        }
    }
}

// Portaudio section

int App::InitAudio() {
    if (Pa_Initialize() != paNoError) {
        printf("PortAudio initialisation failed\n");
        return 1;
    }

    Pa_OpenDefaultStream (
    &_stream,
    0,  // no input
        2,  // stereo output
    paFloat32,  // 32 bit floating point output, I believe ints work as well but this is just better
    44100,   // sample rate
    256,    // frames per buffer - number of frames that portaudio will request from a callback.
    App::AudioCallback, // a callback function
    &_synth
    );

    Pa_StartStream(_stream);

    return 0;
}

int App::AudioCallback (
    const void*,
    void* output,
    const unsigned long frameCount,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void* synthData) {

    auto* synth = static_cast<Synth*>(synthData);

    synth->Run(static_cast<float*>(output), frameCount);

    return paContinue;
}

void App::ShutdownAudio() const {
    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
    Pa_Terminate();
}

// SDL section

int App::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialisation failed: %s\n", SDL_GetError());
        return 1;
    }

    _window = SDL_CreateWindow(
        "Synth",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 200,
        SDL_WINDOW_SHOWN
    );

    _keyNoteMap = {
        {SDLK_a, C4},
        {SDLK_w, Cs4},
        {SDLK_s, D4},
        {SDLK_e, Ds4},
        {SDLK_d, E4},
        {SDLK_f, F4},
        {SDLK_t, Fs4},
        {SDLK_g, G4},
        {SDLK_z, Gs4},
        {SDLK_h, A4},
        {SDLK_u, As4},
        {SDLK_j, B4},
        {SDLK_k, C5},
        {SDLK_o, Cs5},
        {SDLK_l, D5},
        {SDLK_p, Ds5}
    };

    _octave = 0;

    return 0;
}

void App::ShutdownSDL() const {
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

// Event handling

void App::HandleKeyDown(SDL_Keycode key) {
    // octave handler
    if (key == SDLK_y) _octave--;
    if (key == SDLK_x) _octave++;

    // waveform handler
    if (key == SDLK_LEFT || key == SDLK_RIGHT) {
        _synth.CycleWaveform((key == SDLK_RIGHT));
    }

    if (!_keyNoteMap.contains(key)) return;

    NoteId note = ToNoteId(key);
    _heldNotes.push_back(note);
    std::lock_guard<std::mutex> lock(_synthMutex);
    _synth.NoteOn(note);
}

void App::HandleKeyUp(SDL_Keycode key) {
    if (!_keyNoteMap.contains(key)) return;

    NoteId note = ToNoteId(key);

    _heldNotes.erase(std::ranges::remove(_heldNotes, note).begin());

    std::lock_guard<std::mutex> lock(_synthMutex);
    if (!_heldNotes.empty()) {
        _synth.NoteOn(_heldNotes.back());
    }
    else {
        _synth.NoteOff(note);
    }
}
