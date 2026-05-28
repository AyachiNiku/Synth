#pragma once

#include <array>
#include <memory>

#include "voice.hpp"
#include "adsr.hpp"
#include "oscillator.hpp"
#include "delay.hpp"
#include "filter.hpp"

class Synth {
public:
    void Run(float* output, unsigned long sampleRate);
    void CycleWaveform(bool right);
    void CycleWaveform(int oscillatorId, bool right);
    void NoteOn(Note::Id note, float velocity = 127.0f);
    void NoteOff(Note::Id note);
    void AdjustVoiceAmount(bool down);
    void CycleFilter(bool right);

    Synth() {
        _filters.push_back(std::make_unique<LowPassFilter>());
        _filters.push_back(std::make_unique<HighPassFilter>());
        _filters.push_back(std::make_unique<BandPassFilter>());
    }

private:
    std::array<Voice, 8> _voices;
    size_t _disabledVoices = 0;
    size_t _currentVoice = 0;
    Delay _delay;
    std::vector<std::unique_ptr<Filter>> _filters;
    size_t _currentFilter = 0;
};
