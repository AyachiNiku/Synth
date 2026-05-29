#pragma once

#include <portaudio.h>
#include <libremidi/libremidi.hpp>
#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <unordered_map>
#include <mutex>
#include <iostream>

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
    static int AudioCallback(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* synthData);
    PaStream* _stream = nullptr;

    // libremidi section
    std::unique_ptr<libremidi::midi_in> _midiIn = nullptr;
    libremidi::observer _midiObserver;
    void MidiMagic();

    // SDL section
    int InitSDL();
    void ShutdownSDL() const;
    void HandleKeyDown(SDL_Keycode key);
    void HandleKeyUp(SDL_Keycode key);
    SDL_Window* _window = nullptr;
    SDL_GLContext _glContext = nullptr;

    // ImGui section
    void UI();
    void PianoKeyboard();

    // Synth section
    Synth _synth;

    std::unordered_map<SDL_Keycode, Note::Id> _keyNoteMap; // a dictionary of all keys on the keyboard that get mapped to midi notes
    std::vector<Note::Id> _heldNotes; // in App
    std::mutex _synthMutex; // for locking the thread
    int _octave = 0;

    // finds the note to the corresponding key pressed and also based on the currently chosen octave
    Note::Id ToNoteId(SDL_Keycode key) { return _keyNoteMap[key] + (_octave * 12); }
};
