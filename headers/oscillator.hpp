#pragma once
#include "utils.hpp"
#include "waveform.hpp"

class Oscillator {
public:
    Oscillator() = default;

    float Process();
    bool IsActive() const;
    void SetActive(bool active);
    void SetFrequency(float freq);
    void SetWaveform(Waveform waveform);
    void CycleWaveform(bool right);

private:
    Waveform _waveform = Waveform::Sine;

    float _phase = 0.0f;
    float _frequency = 0.0f;
    bool _isActive = true;
};