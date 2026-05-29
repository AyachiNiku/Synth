#include "../headers/app.hpp"

#include "imgui_impl_opengl3_loader.h"

// App section itself

App::~App() {
    App::ShutdownAudio();
    App::ShutdownSDL();
}

void App::Run() {
    // Initialisation
    App::InitAudio();
    App::InitSDL();

    bool running = true;
    SDL_Event event;

    MidiMagic();

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                App::HandleKeyDown(event.key.keysym.sym);
            }
            if (event.type == SDL_KEYUP) {
                App::HandleKeyUp(event.key.keysym.sym);
            }
        }

        // start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        UI();

        // render
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(_window); // vsync
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

void App::ShutdownAudio() const {
    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
    Pa_Terminate();
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

// MIDI section

void App::MidiMagic() {
    // lists MIDI ports - discovery, nothing is being opened yet
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
            _heldNotes.push_back(note);
            _synth.NoteOn(note, velocity);
        }
        else if ((status == 0x80) || (status == 0x90 && velocity == 0)) { // Note Off
            std::lock_guard lock(_synthMutex);
            _heldNotes.erase(std::ranges::remove(_heldNotes, note).begin());
            if (!_heldNotes.empty() && _synth.GetVoiceCount() == 1) {
                _synth.NoteOn(_heldNotes.back());
            } else {
                _synth.NoteOff(note);
            }
        }
    };

    libremidi::input_configuration config;
    config.on_message = midiCallback;

    _midiIn = std::make_unique<libremidi::midi_in>(config);

    // open the first available device
    if (!inputPorts.empty()) {
        _midiIn->open_port(inputPorts[0]);
        std::cout << "[MIDI] Opened input: " << inputPorts[0].port_name << std::endl;
    }
}

// SDL section

int App::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[SDL] Initialisation failed: %s\n", SDL_GetError());
        return 1;
    }

    // SDL shenanigans, flag sets to make SDL create an OpenGL context for ImGui
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    _window = SDL_CreateWindow(
        "Synth",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    _glContext = SDL_GL_CreateContext(_window);
    SDL_GL_MakeCurrent(_window, _glContext);
    SDL_GL_SetSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // enable keyboard controls here
    ImGui_ImplSDL2_InitForOpenGL(_window, _glContext);
    ImGui_ImplOpenGL3_Init();

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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(_glContext);
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

    Note::Id note = ToNoteId(key);
    std::lock_guard lock(_synthMutex);
    _heldNotes.push_back(note);
    _synth.NoteOn(note);
}

void App::HandleKeyUp(SDL_Keycode key) {
    if (!_keyNoteMap.contains(key)) return;

    Note::Id note = ToNoteId(key);
    std::lock_guard lock(_synthMutex);
    _heldNotes.erase(std::ranges::remove(_heldNotes, note).begin());
    if (!_heldNotes.empty() && _synth.GetVoiceCount() == 1) {
        _synth.NoteOn(_heldNotes.back());
    } else {
        _synth.NoteOff(note);
    }
}

// ImGui section

bool IsBlackKey(int semitone) {
    // C#=1, D#=3, F#=6, G#=8, A#=10
    return semitone == 1 || semitone == 3 ||
           semitone == 6 || semitone == 8 || semitone == 10;
}

void App::UI() {
    ImGui::Begin("Synth");

    PianoKeyboard();

    if (ImGui::CollapsingHeader("ADSR")) {
        float attack = _synth.GetAttack();
        float decay = _synth.GetDecay();
        float sustain = _synth.GetSustain();
        float release = _synth.GetRelease();

        if (ImGui::SliderFloat("Attack", &attack, 0.01f, 2.0f))
            _synth.SetAttack(attack);
        if (ImGui::SliderFloat("Decay", &decay, 0.01f, 2.0f))
            _synth.SetDecay(decay);
        if (ImGui::SliderFloat("Sustain", &sustain, 0.0f, 1.0f))
            _synth.SetSustain(sustain);
        if (ImGui::SliderFloat("Release", &release, 0.01f, 3.0f))
            _synth.SetRelease(release);
    }

    if (ImGui::CollapsingHeader("Delay")) {
        bool isActive = _synth.IsDelayActive();
        float feedback = _synth.GetDelayFeedback();
        float mix = _synth.GetDelayMix();
        float delayTime = _synth.GetDelayTime();

        if (ImGui::Checkbox("Enabled", &isActive))
            _synth.SwitchDelayState();
        if (ImGui::SliderFloat("Feedback", &feedback, 0.01f, 1.0f))
            _synth.SetDelayFeedback(feedback);
        if (ImGui::SliderFloat("Mix", &mix, 0.01f, 1.0f))
            _synth.SetDelayMix(mix);
        if (ImGui::SliderFloat("Delay Time", &delayTime, 0.01f, 2.0f))
            _synth.SetDelayTime(delayTime);
    }

    if (ImGui::CollapsingHeader("Filter")) {
        size_t currentFilter = _synth.GetCurrentFilter();
        float filterCutoff = _synth.GetFilterCutoff(currentFilter);
        float filterQ = _synth.GetFilterQ(currentFilter);

        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
            _synth.CycleFilter(false);
        ImGui::SameLine();
        if (ImGui::ArrowButton("##right", ImGuiDir_Right))
            _synth.CycleFilter(true);
        if (ImGui::SliderFloat("Cutoff", &filterCutoff, 0.0f, 22000.0f))
            _synth.SetFilterCutoff(currentFilter, filterCutoff);
        if (ImGui::SliderFloat("Resonance", &filterQ, 0.0f, 2.0f))
            _synth.SetFilterQ(currentFilter, filterQ);
    }

    if (ImGui::CollapsingHeader("Oscillators")) {
        Voice voice = _synth.GetFirstVoice();
        auto oscillators = voice.GetOscillators();
        std::array<bool, 3> areActive = {
            oscillators.at(0).IsActive(),
            oscillators.at(1).IsActive(),
            oscillators.at(2).IsActive(),
        };

        for (size_t i = 0; i < 3; i++) {
            ImGui::PushID(static_cast<int>(i));

            ImGui::Text("Oscillator %d", static_cast<int>(i) + 1);
            ImGui::SameLine();
            if (ImGui::Checkbox("Enabled", &areActive.at(i)))
                _synth.SwitchOscillatorState(i);
            ImGui::SameLine();
            if (ImGui::ArrowButton("##left", ImGuiDir_Left))
                _synth.CycleWaveform(i, false);
            ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right))
                _synth.CycleWaveform(i, true);

            ImGui::PopID();
        }
    }

    ImGui::End();
}

void App::PianoKeyboard() {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetCursorScreenPos();

    constexpr float whiteW = 24.0f;
    constexpr float whiteH = 60.0f;
    constexpr float blackW = whiteW * 0.60f;
    constexpr float blackH = whiteH * 0.65f;
    constexpr int baseNote = 48;
    constexpr int octaves = 2;

    constexpr ImU32 colBorder    = IM_COL32(30, 30, 30, 255);
    constexpr ImU32 colWhite     = IM_COL32(245, 245, 245, 255);
    constexpr ImU32 colWhiteHeld = IM_COL32(170, 210, 255, 255);
    constexpr ImU32 colBlack     = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 colBlackHeld = IM_COL32(90, 150, 220, 255);

    struct BlackKeyInfo { int semitone; int whiteBetween; };
    const BlackKeyInfo blacks[] = {
        { 1, 0 }, { 3, 1 }, { 6, 3 }, { 8, 4 }, { 10, 5 }
    };

    // reserve space immediately so widgets below are unaffected
    ImGui::Dummy(ImVec2(octaves * 7 * whiteW, whiteH));
    ImVec2 cursorAfter = ImGui::GetCursorScreenPos();

    // ---- Pass 1: draw white key rectangles (visual only) ----
    int globalWhite = 0;
    for (int n = 0; n < octaves * 12; n++) {
        int note = baseNote + n;
        int semi = (note % 12 + 12) % 12;
        if (IsBlackKey(semi)) continue;

        const bool held = std::ranges::find(_heldNotes, note) != _heldNotes.end();
        ImVec2 k0 = ImVec2(p0.x + globalWhite * whiteW, p0.y);
        ImVec2 k1 = ImVec2(k0.x + whiteW, p0.y + whiteH);

        dl->AddRectFilled(k0, k1, held ? colWhiteHeld : colWhite);
        dl->AddRect(k0, k1, colBorder);
        globalWhite++;
    }

    // ---- Pass 2: draw black key rectangles + register their buttons ----
    for (int o = 0; o < octaves; o++) {
        for (auto bk : blacks) {
            int note = baseNote + o * 12 + bk.semitone;
            ImGui::PushID(note);

            float xCenter = p0.x + (o * 7 + bk.whiteBetween + 1) * whiteW;
            float x0 = xCenter - blackW * 0.5f;
            const bool held = std::ranges::find(_heldNotes, note) != _heldNotes.end();

            ImVec2 k0 = ImVec2(x0, p0.y);
            ImVec2 k1 = ImVec2(x0 + blackW, p0.y + blackH);

            dl->AddRectFilled(k0, k1, held ? colBlackHeld : colBlack);
            dl->AddRect(k0, k1, colBorder);

            ImGui::SetCursorScreenPos(k0);
            ImGui::InvisibleButton("##black", ImVec2(blackW, blackH));

            if (ImGui::IsItemActivated()) {
                _heldNotes.push_back(note);
                _synth.NoteOn(note);
            }
            if (ImGui::IsItemDeactivated()) {
                _heldNotes.erase(std::ranges::remove(_heldNotes, note).begin(), _heldNotes.end());
                _synth.NoteOff(note);
            }

            ImGui::PopID();
        }
    }

    // ---- Pass 3: register white key buttons (on top, but lose to black keys) ----
    globalWhite = 0;
    for (int n = 0; n < octaves * 12; n++) {
        int note = baseNote + n;
        int semi = (note % 12 + 12) % 12;
        if (IsBlackKey(semi)) continue;

        ImGui::PushID(note + 1000); // offset to avoid ID collision with black keys

        ImVec2 k0 = ImVec2(p0.x + globalWhite * whiteW, p0.y);

        ImGui::SetCursorScreenPos(k0);
        ImGui::InvisibleButton("##white", ImVec2(whiteW, whiteH));

        if (ImGui::IsItemActivated()) {
            _heldNotes.push_back(note);
            _synth.NoteOn(note);
        }
        if (ImGui::IsItemDeactivated()) {
            _heldNotes.erase(std::ranges::remove(_heldNotes, note).begin(), _heldNotes.end());
            _synth.NoteOff(note);
        }

        ImGui::PopID();
        globalWhite++;
    }

    // restore cursor to below keyboard
    ImGui::SetCursorScreenPos(cursorAfter);
}