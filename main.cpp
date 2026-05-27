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
 * - [x] Add support for MIDI input utilizing the libremidi library
 *
 * Part 4 - Voices support
 * - [x] Refactor to an array of voices for polyphony
 *
 * Part 5 - Subtractive synthesis
 * - [ ] Add a low/high pass filter
 * - [x] Consider delay
 *
 * Part 6 - GUI
 * - [ ] Manage to run the project with ImGUI
 * - [ ] Add a component for the keyboard to signal what keys are being played (and the ability to play them)
 * - [ ] Add a component for managing the ADSR envelope
 * - [ ] Add a component showing what Waveform is being used and arrows to switch between them
 * - [ ] Add a component for tweaking the Delay effect, as well as turning it off
 *
 * Part 7 - Tweaks (in GUI)
 * - [ ] Implement ways to turn off certain oscillators
 * - [ ] Implement ways to set the Wavelength to one of the oscillators (not just all of them)
 */

int main() {
    App app;
    app.Run();

    return 0;
}