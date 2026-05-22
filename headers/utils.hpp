#pragma once

#include <cmath>

using NoteId = int;

constexpr NoteId C4  = 60;
constexpr NoteId Cs4 = 61;
constexpr NoteId D4  = 62;
constexpr NoteId Ds4 = 63;
constexpr NoteId E4  = 64;
constexpr NoteId F4  = 65;
constexpr NoteId Fs4 = 66;
constexpr NoteId G4  = 67;
constexpr NoteId Gs4 = 68;
constexpr NoteId A4  = 69;
constexpr NoteId As4 = 70;
constexpr NoteId B4  = 71;
constexpr NoteId C5  = 72;
constexpr NoteId Cs5 = 73;

constexpr float SAMPLE_RATE = 44100.0f;

inline float midiToFrequency(NoteId note) {
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}
