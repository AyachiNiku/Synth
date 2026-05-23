#pragma once

#include <portaudio.h>
#include <libremidi/libremidi.hpp>
#include <SDL2/SDL.h>

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <queue>

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
    std::vector<int> _heldNotes;    // currently held notes on the keyboard
    std::unordered_map<SDL_Keycode, NoteId> _keyNoteMap;    // a dictionary of all keys on the keyboard that get mapped to midi notes
    int _octave = 0;

    // finds the note to the corresponding key pressed and returns its midi id (also based on the currently chosen octave)
    NoteId ToNoteId(SDL_Keycode key) {
        return _keyNoteMap[key] + (_octave * 12);
    }

    std::mutex _synthMutex;
};
