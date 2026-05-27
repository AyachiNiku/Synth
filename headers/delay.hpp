#pragma once

#include <vector>

#include "utils.hpp"

class Delay {
public:
    void Deactivate();
    void Activate();

    Delay(float delaySeconds = 0.5f);

    float Process(float input);

private:
    std::vector<float> _buffer;
    size_t _writePos = 0;
    size_t _readPos = 0;
    float _feedback = 0.5f; // how much the echo repeats
    float _mix = 0.33f;      // dry/wet balance

    bool _isActive = true;
};
