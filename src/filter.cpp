#include "../headers/filter.hpp"

float Filter::Process(float input) {
    if (!_isActive) return input;

    float output = _coefficients.b0 * input
                 + _coefficients.b1 * _x1
                 + _coefficients.b2 * _x2
                 - _coefficients.a1 * _y1
                 - _coefficients.a2 * _y2;

    // shift sample history
    _x2 = _x1;
    _x1 = input;
    _y2 = _y1;
    _y1 = output;

    return output;
}

void Filter::SetCutoff(float cutoffHz) {
    _cutoff = cutoffHz;
    CalculateCoefficients();
}

void Filter::SetQ(float q) {
    _q = q;
    CalculateCoefficients();
}

bool Filter::IsActive() const {
    return _isActive;
}

float Filter::GetCutoff() const {
    return _cutoff;
}

float Filter::GetQ() const {
    return _q;
}

void Filter::Enable() {
    _isActive = true;
}

void Filter::Disable() {
    _isActive = false;
}

void Filter::SwitchState() {
    _isActive = !_isActive;
}

// LPF

void LowPassFilter::CalculateCoefficients() {
    float w0 = 2.0f * static_cast<float>(M_PI) * GetCutoff() / SAMPLE_RATE; // conversion to 0-PI range
    float alpha = std::sin(w0) / (2.0f * GetQ());

    float a0 =  1.0f + alpha;

    _coefficients.b0 = ((1.0f - std::cos(w0)) / 2.0f)   / a0;
    _coefficients.b1 = (1.0f - std::cos(w0))            / a0;
    _coefficients.b2 = ((1.0f - std::cos(w0)) / 2.0f)   / a0;
    _coefficients.a1 = (-2.0f * std::cos(w0))           / a0;
    _coefficients.a2 = (1.0f - alpha)                   / a0;
}

void LowPassFilter::SetCutoff(float cutoffHz) {
    _cutoff = cutoffHz;
    CalculateCoefficients();
}

void LowPassFilter::SetQ(float q) {
    _q = q;
    CalculateCoefficients();
}

// HPF

void HighPassFilter::CalculateCoefficients() {
    float w0 = 2.0f * static_cast<float>(M_PI) * GetCutoff() / SAMPLE_RATE; // conversion to 0-PI range
    float alpha = std::sin(w0) / (2.0f * GetQ());

    float a0 =  1.0f + alpha;

    _coefficients.b0 = ((1.0f + std::cos(w0)) / 2.0f)   / a0;
    _coefficients.b1 = (-(1.0f + std::cos(w0)))         / a0;
    _coefficients.b2 = ((1.0f + std::cos(w0)) / 2.0f)   / a0;
    _coefficients.a1 = (-2.0f * std::cos(w0))           / a0;
    _coefficients.a2 = (1.0f - alpha)                   / a0;
}

void HighPassFilter::SetCutoff(float cutoffHz) {
    _cutoff = cutoffHz;
    CalculateCoefficients();
}

void HighPassFilter::SetQ(float q) {
    _q = q;
    CalculateCoefficients();
}

// BPF

void BandPassFilter::CalculateCoefficients() {
    float w0 = 2.0f * static_cast<float>(M_PI) * GetCutoff() / SAMPLE_RATE; // conversion to 0-PI range
    float alpha = std::sin(w0) / (2.0f * GetQ());

    float a0 =  1.0f + alpha;

    _coefficients.b0 = _q * alpha             / a0;
    _coefficients.b1 = 0.0f                   / a0;
    _coefficients.b2 = -_q * alpha            / a0;
    _coefficients.a1 = (-2.0f * std::cos(w0)) / a0;
    _coefficients.a2 = (1.0f - alpha)         / a0;
}

void BandPassFilter::SetCutoff(float cutoffHz) {
    _cutoff = cutoffHz;
    CalculateCoefficients();
}

void BandPassFilter::SetQ(float q) {
    _q = q;
    CalculateCoefficients();
}
