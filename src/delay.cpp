#include "../headers/delay.hpp"

Delay::Delay() {
    _buffer.resize(static_cast<int>(2.0f * SAMPLE_RATE), 0.0f);
    SetDelayTime(0.5f); // default 500ms
}

float Delay::Process(float input) {
    if (!_isActive) return input;

    // read the delayed sample
    float delayed = _buffer[_readPos];

    // write input + feedback into the buffer
    _buffer[_writePos] = input + delayed * _feedback;

    // advance positions
    _writePos = (_writePos + 1) % _buffer.size();
    _readPos = (_readPos + 1) % _buffer.size();

    // mix dry and wet signal
    return input * (1.0f - _mix) + delayed * _mix;
}

// setters

void Delay::SetDelayTime(float seconds) {
    _delayTime = std::clamp(seconds, 0.01f, 2.0f);
    _delaySamples = static_cast<int>(_delayTime * SAMPLE_RATE);
    // recalculate read position relative to write position
    _readPos = static_cast<size_t>((_writePos - _delaySamples + _buffer.size())) % _buffer.size();
}
