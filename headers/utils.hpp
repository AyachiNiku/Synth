#pragma once

#include <cmath>

constexpr float SAMPLE_RATE = 44100.0f;

// used to represent the notes that are being played
// for convenience, I represent them like so - each note directly equals its midi numbered counterpart
namespace Note {
    using Id = int;

    constexpr Id C4  = 60;
    constexpr Id Cs4 = 61;
    constexpr Id D4  = 62;
    constexpr Id Ds4 = 63;
    constexpr Id E4  = 64;
    constexpr Id F4  = 65;
    constexpr Id Fs4 = 66;
    constexpr Id G4  = 67;
    constexpr Id Gs4 = 68;
    constexpr Id A4  = 69;
    constexpr Id As4 = 70;
    constexpr Id B4  = 71;
    constexpr Id C5  = 72;
    constexpr Id Cs5 = 73;
    constexpr Id D5  = 74;
    constexpr Id Ds5 = 75;

    // midi notes are nice, because there's also a normalized method to convert them to their respective frequencies, like so:
    inline float ToFrequency(Note::Id note) {
        return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69) / 12.0f);
    }

}
