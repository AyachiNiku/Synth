#include <cmath>
#include "../headers/oscillator.hpp"

void Oscillator::CycleWaveform(bool right) {
    auto current = static_cast<int>(_waveform);

    if (right) {
        current = (current == 3) ? 0 : current + 1;
    }
    else {
        current = (current == 0) ? 3 : current - 1;
    }

    _waveform = static_cast<Waveform>(current);
}

float Oscillator::Process() {
    float sample = 0.0f;

    switch (_waveform) {
        case Waveform::Sine:
            sample = static_cast<float>(std::sin(_phase * 2.0f * M_PI));
            break;

        case Waveform::Square:
            sample = _phase < 0.5f ? 1.0f : -1.0f;
            break;

        case Waveform::Triangle:
            sample = _phase < 0.5f
                ? (4.0f * _phase - 1.0f)
                : (3.0f - 4.0f * _phase);
            break;

        case Waveform::Sawtooth:
            sample = 2.0f * _phase - 1.0f;
            break;
    }

    _phase += _frequency / SAMPLE_RATE;
    if (_phase >= 1.0f) _phase -= 1.0f;

    return sample;
}
