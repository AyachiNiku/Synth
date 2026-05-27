#include "../headers/filter.hpp"

Filter::Filter() {
    SetCutoff(8000.0f);
}

void Filter::CalculateCoefficients() {
    float w0 = 2.0f * static_cast<float>(M_PI) * _cutoff / SAMPLE_RATE; // conversion to 0-PI range
    float alpha = std::sin(w0) / (2.0f * _q);

    float b0 = (1.0f - std::cos(w0)) / 2.0f;
    float b1 =  1.0f - std::cos(w0);
    float b2 = (1.0f - std::cos(w0)) / 2.0f;
    float a0 =  1.0f + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 =  1.0f - alpha;

    // normalize by a0
    _b0 = b0 / a0;
    _b1 = b1 / a0;
    _b2 = b2 / a0;
    _a1 = a1 / a0;
    _a2 = a2 / a0;
}

void Filter::SetCutoff(float cutoffHz) {
    _cutoff = cutoffHz;
    CalculateCoefficients();
}

void Filter::SetQ(float q) {
    _q = q;
    CalculateCoefficients();
}

float Filter::Process(float input) {
    if (!_isActive) return input;

    float output = _b0 * input
                 + _b1 * _x1
                 + _b2 * _x2
                 - _a1 * _y1
                 - _a2 * _y2;

    // shift sample history
    _x2 = _x1;
    _x1 = input;
    _y2 = _y1;
    _y1 = output;

    return output;
}

void Filter::Disable() {
    _isActive = false;
}

void Filter::Enable() {
    _isActive = true;
}
