#include <cmath>
#include "../headers/oscillator.hpp"

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
