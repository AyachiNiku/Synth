#pragma once

#include <algorithm>
#include <vector>

#include "utils.hpp"

class Delay {
public:
    Delay();

    float Process(float input);

    // setters
    void SwitchState() { _isActive = !_isActive; }
    void SetFeedback(float feedback) { _feedback = feedback; }
    void SetMix(float mix) { _mix = mix; }
    void SetDelayTime(float seconds);

    // getters
    bool IsActive() const { return _isActive; }
    float GetFeedback() const { return _feedback; }
    float GetMix() const { return _mix; }
    float GetDelayTime() const { return _delayTime; }

private:
    std::vector<float> _buffer;
    size_t _writePos = 0;
    size_t _readPos = 0;
    float _feedback = 0.5f; // how much the echo repeats
    float _mix = 0.33f;      // dry/wet balance
    float _delayTime = 0.5f;
    float _delaySamples = 0.5f * SAMPLE_RATE;

    bool _isActive = true;
};
