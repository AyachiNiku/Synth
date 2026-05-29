#pragma once

#include "utils.hpp"

// source: https://www.w3.org/TR/audio-eq-cookbook/
// specifically check the "Direct Form 1" section:
// y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
// and "LPF" (Los-Pass Filter) section, where the desired coefficients are defined:
// b0 = (1 - cos(w0)) / 2
// b1 =  1 - cos(w0)
// b2 = (1 - cos(w0)) / 2
// a0 =  1 + alpha
// a1 = -2 * cos(w0)
// a2 =  1 - alpha
//
// where:
// w0 = 2 * PI * cutoff / sampleRate (where sampleRate
// alpha = sin(w0) / (2 * Q)

// THIS IS ME RANTING ABOUT SOME THINGS I WAS INTERESTED IN THE EQUATION;
// IT'S PRETTY INTERESTING WHAT'S GOING ON IN THERE

// w0 - angular or normalized frequency
// it's basically the cutoff frequency converted to radians per sample for math reasons
// instead of cycles per second (Hz)

// for example, a 1000 Hz tone = 1000 cycles in a second;
// at 44100 sample rate, we get: 1000 / 44100
// cycles per sample => radians per sample - in radians, 2*PI is a cycle, so:
// w0 = 2*PI * (cutoff / sampleRate)

// there's this quirk that at 44100 sample rate, the highest frequency possible
// to represent is exactly half of it, 22050. This means that this is "just"
// unit conversion - at 0 Hz we're at 0, at this highest "Nyquist" frequency, we're at PI.

// the coefficients themselves are also quite interesting - they contain a lot of cosines,
// which ends up making sense in the end;
// we converted the cutoff frequency to a point rotating around a unit circle via above,
// cosines give the horizontal component, maximum (1.0) equaling 0 radians, minimum (0.0) equaling PI (Nyquist frequency)

// at w0 being a low value - cos(w0) becomes high and in b1 = 1 - cos(w0) - barely anything passes the filter
// the other way around is trivial with this knowledge

// alpha is the width of the transition band - so how sharply the filter cuts through the frequencies
// in alpha = sin(w0) / (2 * Q), high Q makes alpha small => sharp transition, vice versa in the opposite case

// OKAY RANT OVER

struct Coefficients {
    float b0 = 0.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
};

class Filter {
public:
    Filter() = default;
    virtual ~Filter() = default;

    virtual float Process(float input);

    void SwitchState() { _isActive = !_isActive; }
    virtual void SetCutoff(float cutoffHz) = 0;
    virtual void SetQ(float q) = 0;

    bool IsActive() const { return _isActive; }
    float GetCutoff() const { return _cutoff; }
    float GetQ() const { return _q; }

protected:
    virtual void CalculateCoefficients() = 0;
    float BiquadProcess(float input);

    float _cutoff = 500.0f;
    float _q = 0.707f;
    float _x1 = 0.0f, _x2 = 0.0f;
    float _y1 = 0.0f, _y2 = 0.0f;
    Coefficients _coefficients;

    bool _isActive = true;
};

class LowPassFilter : public Filter {
public:
    LowPassFilter() { SetCutoff(5000.0f); }

    void SetCutoff(float cutoffHz) override;
    void SetQ(float q) override;

protected:
    void CalculateCoefficients() override;
};

class HighPassFilter : public Filter {
public:
    HighPassFilter() { SetCutoff(2500.0f); }

    void SetCutoff(float cutoffHz) override;
    void SetQ(float q) override;

protected:
    void CalculateCoefficients() override;
};

class BandPassFilter : public Filter {
public:
    BandPassFilter() { SetCutoff(2500.0f); }

    void SetCutoff(float cutoffHz) override;
    void SetQ(float q) override;

protected:
    void CalculateCoefficients() override;
};