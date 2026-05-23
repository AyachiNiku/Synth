#include "headers/app.hpp"

/**
 * Roadmap
 *
 * Part 1 - Introduction
 * - [x] Produce a basic sine wave to get started
 * - [x] refactor to an oscillator class that can receive frequency
 *
 * Part 2 - Envelope
 * - [x] Add a ADSR envelope that makes the sound a lot smoother
 *
 * Part 3 - Keyboard support
 * - [x] Add support for keyboard input utilizing the SDL library
 * - [ ] Add support for MIDI input utilizing the libremidi library
 *
 * Part 4 - Voices support
 * - [ ] Refactor to an array of voices for polyphony
 *
 * Part 5 - Subtractive synthesis
 * - [ ] add a low pass filter
 * - [ ] add a high pass filter
 * - [ ] consider delay
 * - [ ] consider reverb
 * - [ ] room for many others
 */

int main() {
    App app;
    app.Run();

    return 0;
}