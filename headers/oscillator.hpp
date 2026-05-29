#pragma once

#include "utils.hpp"
#include "waveform.hpp"

class Oscillator {
public:
    Oscillator() = default;

    float Process();

    void SwitchState() { _isActive = !_isActive; }
    void SetFrequency(float freq) { _frequency = freq; }
    void CycleWaveform(bool right);

    bool IsActive() const { return _isActive; }
    Waveform& GetWaveform() { return _waveform; }

private:
    Waveform _waveform = Waveform::Sine;

    float _phase = 0.0f;
    float _frequency = 0.0f;
    bool _isActive = true;
};