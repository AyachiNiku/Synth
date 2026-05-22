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
private:
    ADSRState _state = ADSRState::Idle;
    float _amplitude = 0.0f;    // between 0.0f and 1.0f

    // variables which dictate how much/quick the output is increased/decreased,
    // or at which level they stay
    float _attackTime = 0.1f;
    float _decayTime = 0.3f;
    float _sustainLevel = 0.8f;
    float _releaseTime = 0.5f;

public:
    float Process();

    void NoteOn() { _state = ADSRState::Attack; }
    void NoteOff() { _state = ADSRState::Release; }

    void SetAttackTime(float rate);
    void SetDecayTime(float rate);
    void SetSustainLevel(float level);
    void SetReleaseTime(float rate);
};