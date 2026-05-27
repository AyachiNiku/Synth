#pragma once

#include <array>

#include "voice.hpp"
#include "adsr.hpp"
#include "oscillator.hpp"
#include "delay.hpp"

class Synth {
private:
    std::array<Voice, 8> _voices;
    size_t _disabledVoices = 0;
    size_t _currentVoice = 0;
    Delay _delay;

public:
    void Run(float* output, unsigned long sampleRate);
    void CycleWaveform(bool right);
    void CycleWaveform(int oscillatorId, bool right);
    void NoteOn(Note::Id note, float velocity = 127.0f);
    void NoteOff(Note::Id note);
    void AdjustVoiceAmount(bool down);

    Synth() = default;
};