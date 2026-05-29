#pragma once

#include <array>

#include "adsr.hpp"
#include "oscillator.hpp"

class Voice {
public:
    Voice() = default;

    float Process();

    void NoteOn(Note::Id note, float velocity);
    void NoteOff(Note::Id note);

    void CycleWaveform(bool right) { for (auto& osc : _oscillators) osc.CycleWaveform(right); }
    void CycleWaveform(size_t oscillatorId, bool right) { _oscillators.at(oscillatorId).CycleWaveform(right); }

    void Disable() { _isDisabled = true; }
    void Enable() { _isDisabled = false; }

    bool IsNoteOn() const { return _isNoteOn; }
    bool IsActive() const { return _isActive; }
    bool IsDisabled() const { return _isDisabled; }
    Note::Id GetNote() const { return _note; }

    // getters
    float GetAttack() const { return _adsr.GetAttack(); }
    float GetDecay() const { return _adsr.GetDecay(); }
    float GetSustain() const { return _adsr.GetSustain(); }
    float GetRelease() const { return _adsr.GetRelease(); }
    std::array<Oscillator, 3> GetOscillators() const { return _oscillators; }

    // setters
    void SwitchOscillatorState(size_t oscillatorId) { _oscillators.at(oscillatorId).SwitchState(); }
    void SetAttack(float attackTime) { _adsr.SetAttack(attackTime); }
    void SetDecay(float decayTime) { _adsr.SetDecay(decayTime); }
    void SetSustain(float sustainLevel) { _adsr.SetSustain(sustainLevel); }
    void SetRelease(float releaseTime) { _adsr.SetRelease(releaseTime); }

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
};
