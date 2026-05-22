#include "headers/app.hpp"

/**
 * Roadmap
 *
 * Part 1
 * - [x] Produce a basic sine wave to get started
 * - [x] refactor to an oscillator class that can receive frequency
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

int main() {
    App app;
    app.Run();

    return 0;
}