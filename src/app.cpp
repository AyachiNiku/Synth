#include "../headers/app.hpp"
#include <iostream>

// App section itself
Note::Id App::ToNoteId(SDL_Keycode key) {
    return _keyNoteMap[key] + (_octave * 12);
}

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
    libremidi::observer _midiObserver;
    auto inputPorts = _midiObserver.get_input_ports();

    if (inputPorts.empty()) {
        std::cout << "[MIDI] No input devices found.\n";
    } else {
        std::cout << "[MIDI] Available input ports:\n";
        int idx = 0;
        for (const auto& port : inputPorts) {
            std::cout << "  [" << idx++ << "] " << port.port_name << std::endl;
        }
    }

    // a lambda callback used for MIDI - this is passed to capture the App object,
    // so that the _synth member functions can be called later

    // bytes are decoded like this:
        // msg.bytes[0] - status (type of message) and the channel packed together
        // msg.bytes[1] - note number (0-127)
        // msg.bytes[2] - velocity (0-127)
    auto midiCallback = [this](const libremidi::message& msg)
    {
        // MIDI message data is in msg.bytes
        if (msg.size() < 3) return; // ignore non-note messages

        auto status = msg.bytes[0] & 0xf0;  // extracts the top 4 bits (status)
        // channel could be extracted from the bottom 4 bits, unused
        auto note = msg.bytes[1];
        auto velocity = msg.bytes[2];

        if (status == 0x90 && velocity > 0) { // Note On
            std::lock_guard lock(_synthMutex);
            this->_synth.NoteOn(note, velocity);
        }
        else if ((status == 0x80) || (status == 0x90 && velocity == 0)) { // Note Off
            std::lock_guard lock(_synthMutex);
            this->_synth.NoteOff(note);
        }
    };

    libremidi::input_configuration config;
    config.on_message = midiCallback;

    libremidi::midi_in midiIn{config};

    // open the first available device
    if (!inputPorts.empty()) {
        midiIn.open_port(inputPorts[0]);
        std::cout << "[MIDI] Opened input: " << inputPorts[0].port_name << std::endl;
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::cout << "[KEY] Quitting the app." << std::endl;
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
        printf("[PortAudio] Initialisation failed\n");
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
        printf("[SDL] Initialisation failed: %s\n", SDL_GetError());
        return 1;
    }

    _window = SDL_CreateWindow(
        "Synth",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 200,
        SDL_WINDOW_SHOWN
    );

    using namespace Note;
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

    // filter handler
    if (key == SDLK_1 || key == SDLK_2) {
        _synth.CycleFilter((key == SDLK_2));
    }

    // waveform handler
    if (key == SDLK_LEFT || key == SDLK_RIGHT) {
        _synth.CycleWaveform((key == SDLK_RIGHT));
    }
    if (key == SDLK_UP || key == SDLK_DOWN) {
        _synth.AdjustVoiceAmount(key == SDLK_DOWN);
    }

    if (!_keyNoteMap.contains(key)) return;
    
    std::lock_guard lock(_synthMutex);
    _synth.NoteOn(ToNoteId(key));
}

void App::HandleKeyUp(SDL_Keycode key) {
    if (!_keyNoteMap.contains(key)) return;

    std::lock_guard lock(_synthMutex);
    _synth.NoteOff(ToNoteId(key));
}
