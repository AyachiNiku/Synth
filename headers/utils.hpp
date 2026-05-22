#pragma once

#include <cmath>

// used to represent the notes that are being played
// for convenience, I represent them like so - each note directly equals its midi numbered counterpart

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
constexpr NoteId D5  = 74;
constexpr NoteId Ds5 = 75;

constexpr float SAMPLE_RATE = 44100.0f;

// midi notes are nice, because there's also a normalized method to convert them to their respective frequencies, like so:
inline float midiToFrequency(NoteId note) {
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}
