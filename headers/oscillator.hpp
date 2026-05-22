#pragma once
#include "utils.hpp"

enum class Waveform {
    Sine = 0,
    Square = 1,
    Triangle = 2,
    Sawtooth = 3,
};

class Oscillator {
private:
    Waveform _waveform;

    float _phase;
    float _frequency;
    bool _active;

public:
    float Process();
    bool IsActive() const { return _active; }
    void SetActive(bool active) { this->_active = active; }
    void SetFrequency(float freq) { this->_frequency = freq; }
    void SetWaveform(Waveform waveform) { this->_waveform = waveform; }

    Oscillator() : _waveform{Waveform::Sine}, _phase{0.0f}, _frequency{440.0f}, _active{true} {}
    Oscillator(float frequency, bool active = true) : _waveform{Waveform::Sine}, _phase{0.0f}, _frequency{frequency}, _active{active} {}
    Oscillator(float frequency, Waveform waveform, bool active = true) : _waveform{waveform}, _phase{0.0f}, _frequency{frequency}, _active{active} {}
};