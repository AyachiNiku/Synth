#include "../headers/adsr.hpp"

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

// for later: if I consider using exponential curves instead of
// linear lines for transitions of these, this is where target ratios step in
// instead of amplitude += amount, I'd use:
// amplitude += coef * amplitude + base,
// and the target ratio would control how close the curve gets to its target

float ADSR::Process() {
    switch (_state) {
        case ADSRState::Attack:
            _amplitude += 1.0f / (_attackTime * SAMPLE_RATE);
            if (_amplitude >= 1.0f) {
                _amplitude = 1.0f;
                _state = ADSRState::Decay;
            }
            break;
        case ADSRState::Decay:
            _amplitude -= (1.0f - _sustainLevel) / (_decayTime * SAMPLE_RATE);
            if (_amplitude <= _sustainLevel) {
                _amplitude = _sustainLevel;
                _state = ADSRState::Sustain;
            }
            break;
        case ADSRState::Sustain:
            break; // just holds _amplitude at _sustainLevel
        case ADSRState::Release:
            _amplitude -= _amplitude / (_releaseTime * SAMPLE_RATE);
            if (_amplitude <= 0.0f) {
                _amplitude = 0.0f;
                _state = ADSRState::Idle;
            }
            break;
        case ADSRState::Idle:
            break;
    }
    return _amplitude;
}
