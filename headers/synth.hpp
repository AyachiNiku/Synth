#pragma once

#include <array>

#include "adsr.hpp"
#include "oscillator.hpp"

class Synth {
private:
    float _frequency = 440.0f;
    int _octave = 1;

    std::array<Oscillator, 3> _oscillators = {
        Oscillator(440.0f, Waveform::Square),
        Oscillator(880.0f),
        Oscillator(0.0f, false)
    };

public:
    ADSR Adsr;
    void Run(float* output, unsigned long sampleRate);

    void SetFrequency(float frequency);
    void CycleWaveform(bool right);

    Synth() = default;
};