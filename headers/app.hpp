#pragma once

#include <SDL2/SDL.h>
#include <portaudio.h>

#include <unordered_map>
#include <vector>
#include <algorithm>

#include "synth.hpp"
#include "utils.hpp"

class App {
public:
    App() = default;
    ~App();
    void Run();

private:
    // Portaudio section
    int InitAudio();
    void ShutdownAudio() const;
    static int AudioCallback(const void*, void*, unsigned long,
                             const PaStreamCallbackTimeInfo*,
                             PaStreamCallbackFlags, void* synthData);
    PaStream* _stream = nullptr;

    // SDL section
    int InitSDL();
    void ShutdownSDL() const;
    void HandleKeyDown(SDL_Keycode key);
    void HandleKeyUp(SDL_Keycode key);
    SDL_Window* _window = nullptr;

    // Synth section
    Synth _synth;

    // Key state section
    std::vector<int> _heldNotes;
    std::unordered_map<SDL_Keycode, NoteId> _keyNoteMap;
    int _octave = 0;

    NoteId ToNoteId(SDL_Keycode key) {
        return _keyNoteMap[key] + (_octave * 12);
    }
};
