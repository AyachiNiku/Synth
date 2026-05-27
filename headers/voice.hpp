#pragma once

#include <array>

#include "adsr.hpp"
#include "oscillator.hpp"

class Voice {
private:
    std::array<Oscillator, 3> _oscillators = {
        Oscillator(),
        Oscillator(),
        Oscillator()
    };

    ADSR _adsr;

    Note::Id _note = -1;
    float _velocity = 1.0f;

    bool _isNoteOn = false; // is a note still being pressed?
    bool _isActive = false; // is sound still playing?
    bool _isDisabled = false; // can this voice even generate any sound right now?

public:
    Voice() = default;

    void CycleWaveform(bool right);
    void CycleWaveform(int oscillatorId, bool right);
    float Process();
    void NoteOn(Note::Id note, float velocity);
    void NoteOff(Note::Id note);

    void Disable();
    void Enable();

    bool IsNoteOn() const;
    bool IsActive() const;
    bool IsDisabled() const;
    Note::Id GetNote() const;
};
