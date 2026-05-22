#include "../headers/adsr.hpp"

// setters

void ADSR::SetAttackTime(float seconds) {
    _attackTime = std::max(seconds, 0.001f);
}

void ADSR::SetDecayTime(float rate) {
    _decayTime = std::max(rate, 0.001f);
}

void ADSR::SetReleaseTime(float rate) {
    _releaseTime = std::max(rate, 0.001f);
}

void ADSR::SetSustainLevel(float level) {
    _sustainLevel = level;
}

/**
 * for later: if I consider using exponential curves instead of
 * linear lines for transitions of these, this is where target ratios step in
 * instead of amplitude += amount, I'd use something like:
 * amplitude += coef * amplitude + base,
 * and the target ratio would control how close the curve gets to its target
 */

// the concept of time and rate (like attackTime/attackRate, decayTime/decayRate, etc.) might seem pretty difficult to grasp,
// the easiest way is to realize that multiplying any time variable by the sample rate gives us the actual time in seconds
// so _attackRate = 0.1f - this gives us 0.1 seconds when multiplied by the current sample rate

float ADSR::Process() {
    switch (_state) {
        // a note has been pressed, and we enter the attack state:
        case ADSRState::Attack:
            _amplitude += 1.0f / (_attackTime * SAMPLE_RATE);   // we rise with the amplitude with 1 / attackRate
            if (_amplitude >= 1.0f) {
                _amplitude = 1.0f;
                _state = ADSRState::Decay;
            }
            break;
        // the peak of the amplitude has been reaches, and we switch to the decay state:
        case ADSRState::Decay:
            _amplitude -= (1.0f - _sustainLevel) / (_decayTime * SAMPLE_RATE);
            if (_amplitude <= _sustainLevel) {
                _amplitude = _sustainLevel;
                _state = ADSRState::Sustain;
            }
            break;
        // the amplitude has reached the designated sustain level - nothing happens, the note is still being held down, and we stay at this volume
        case ADSRState::Sustain:
            break;
        // no keys are being held, and we have entered the release state:
        case ADSRState::Release:
            _amplitude -= _amplitude / (_releaseTime * SAMPLE_RATE);
            if (_amplitude <= 0.0f) {
                _amplitude = 0.0f;
                _state = ADSRState::Idle;
            }
            break;
        // once the amplitude reaches 0, we go back to the idle state
        case ADSRState::Idle:
            break;
    }
    return _amplitude;
}
