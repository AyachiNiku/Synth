#include "../headers/delay.hpp"

bool Delay::IsActive() const {
    return _active;
}

void Delay::Deactivate() {
    _active = false;
}

void Delay::Activate() {
    _active = true;
}

Delay::Delay(float delaySeconds) {
    int delaySamples = static_cast<int>(delaySeconds * SAMPLE_RATE);
    _buffer.resize(delaySamples, 0.0f);
    _writePos = 0;
    _readPos = 0;
}

float Delay::Process(float input) {
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