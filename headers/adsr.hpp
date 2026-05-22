#pragma once

#include <algorithm>
#include "config.hpp"

enum class ADSRState {
    Idle = 0,       // do nothing (output is at zero)
    Attack = 1,     // continue to increase the output at an attack rate (at maximum, change to decay)
    Decay = 2,      // continue to reduce the output at a decay rate
    Sustain = 3,    // do nothing (output is at the sustain level)
    Release = 4,    // continue to reduce the output at release rate (at zero, change to idle)
};

// ~ the flow of the ADSR envelope ~

// when a key is pressed, that's when a "gate" is opened - sets to an "on" state,
// triggering the envelope into its initial attack state

// once the attack state reaches its peak, it enters the decay state

// once the decay state is reached, here we check whether the key is still pressed (gate is still opened)
// if yes - we stay in the sustain state
// if not - we enter the release state

// keep decreasing the output until we reach the idle state

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