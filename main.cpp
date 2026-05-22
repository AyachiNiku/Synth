#include <iostream>
#include <portaudio.h>
#include <set>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "headers/synth.hpp"
#include "headers/notes.hpp"

/**
 * Roadmap
 *
 * Part 1
 * - [x] Produce a basic sine wave to get started
 * - [ ] refactor to an oscillator class that can receive frequency
 *
 * Part 2 - playability
 * - [ ] add the keyboard input suport, mapping to frequencies
 * - [ ] voice class (?)
 *
 * Part 3 - Envelope
 *
 * Part 4 - Subtractive synthesis
 * - [ ] add a low pass filter
 * - [ ] add a high pass filter
 * - [ ] consider delay
 * - [ ] consider reverb
 * - [ ] room for many others
 *
 * Part 5 - Optional features
 * - [ ] multiple voices
 * - [ ] GUI
 * - [ ] add midi support
 */

float midiToFrequency(int note) {
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

static int audioCallback (
    const void*,
    void* output,
    const unsigned long frameCount,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void* userData) {

    auto* synth = static_cast<Synth*>(userData);

    synth->Run(static_cast<float*>(output), frameCount);

    return paContinue;
}

std::set<SDL_Keycode> heldKeys;

std::unordered_map<SDL_Keycode, int> keyNoteMap = {
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
};
int octave = 0;

int main() {
    if (Pa_Initialize() != paNoError) {
        printf("PortAudio initialisation failed\n");
        return 1;
    }

    PaStream *stream;
    Synth synth;

    // open an audio IO stream
    Pa_OpenDefaultStream (
        &stream,
        0,  // no input
            2,  // stereo output
        paFloat32,  // 32 bit floating point output, i believe ints work as well but this is just better
        44100,   // sample rate
        256,    // frames per buffer - number of frames that portaudio will request from a callback.
        audioCallback, // a callback function
        &synth
    );

    Pa_StartStream(stream);

    // ~ SDL part of the setup ~
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialisation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Synth",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 200,
        SDL_WINDOW_SHOWN
    );

    // ~ The actual event loop ~
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_y) octave--;
                if (event.key.keysym.sym == SDLK_x) octave++;
                heldKeys.insert(event.key.keysym.sym);
                if (keyNoteMap.count(event.key.keysym.sym)) {
                    int note = keyNoteMap[event.key.keysym.sym] + (octave * 12);
                    float freq = midiToFrequency(note);
                    synth.SetFrequency(freq);
                    synth.Adsr.NoteOn();
                }
            }
            if (event.type == SDL_KEYUP) {
                heldKeys.erase(event.key.keysym.sym);
                if (heldKeys.empty()) {
                    synth.Adsr.NoteOff();
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}