#pragma once

#include "utils.hpp"

enum class ADSRState {
    Idle = 0,       // do nothing (output is at zero)
    Attack = 1,     // continue to increase the output at an attack rate (at maximum, change to decay)
    Decay = 2,      // continue to reduce the output at a decay rate
    Sustain = 3,    // do nothing (output is at the sustain level)
    Release = 4,    // continue to reduce the output at release rate (at zero, change to idle)
};

class ADSR {
public:
    ADSR() = default;

    float Process();

    // a note has been pressed, we enter the attack state
    void NoteOn() { _state = ADSRState::Attack; }
    // not a single note is pressed at this point in time, we enter the release state
    void NoteOff() { _state = ADSRState::Release; }


    // setters
    void SetAttack(float attackTime) { _attackTime = std::max(attackTime, 0.001f); }
    void SetDecay(float decayTime) { _decayTime = std::max(decayTime, 0.001f); }
    void SetSustain(float sustainLevel) { _sustainLevel = std::max(sustainLevel, 0.001f); }
    void SetRelease(float releaseTime) { _releaseTime = releaseTime; }

    // getters
    ADSRState GetState() const { return _state; }
    float GetAttack() const { return _attackTime; }
    float GetDecay() const { return _decayTime; }
    float GetSustain() const { return _sustainLevel; }
    float GetRelease() const { return _releaseTime; }

private:
    ADSRState _state = ADSRState::Idle;
    float _amplitude = 0.0f;    // between 0.0f and 1.0f, this determines how "strong" (loud) the signal is

    // variables which dictate how much/quick the output is increased/decreased,
    // or at which level they stay
    float _attackTime = 0.05f;
    float _decayTime = 0.3f;
    float _sustainLevel = 0.8f;
    float _releaseTime = 0.1f;
};