#pragma once

#include <portaudio.h>
#include <set>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "synth.hpp"

class App {
public:
    App();
    ~App();
    int Run();

private:
    // Portaudio section
    int InitAudio();
    void ShutdownAudio();
    static int AudioCallback(const void*, void*, unsigned long,
                             const PaStreamCallbackTimeInfo*,
                             PaStreamCallbackFlags, void*);
    PaStream* _stream = nullptr;

    // SDL section
    void InitSDL();
    void ShutdownSDL();
    void HandleKeyDown(SDL_Keycode key);
    void HandleKeyUp(SDL_Keycode key);
    SDL_Window* _window = nullptr;

    // Synth section
    Synth _synth;

    // Key state section
    std::set<SDL_Keycode> _heldKeys;
    std::unordered_map<SDL_Keycode, int> _keyNoteMap;
    int _octave = 0;
};
