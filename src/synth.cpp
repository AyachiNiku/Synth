#include "../headers/synth.hpp"

void Synth::CycleWaveform(bool right) {
    for (int i = 0; i < 3; i++) {
        _oscillators[i].CycleWaveform(right);
    }
}

void Synth::SetFrequency(float frequency) {
    _oscillators[0].SetFrequency(frequency);
    _oscillators[1].SetFrequency(frequency);
    _oscillators[2].SetFrequency(frequency);
}

void Synth::NoteOn(int note, float velocity) {
    SetFrequency(midiToFrequency(note));
    Adsr.NoteOn();
}

void Synth::NoteOff(int note) {
    Adsr.NoteOff();
}

void Synth::Run(float* output, unsigned long sampleRate) {
    // for sample in sampleRate,
    for (unsigned long i = 0; i < sampleRate; i++) {
        float sample = _oscillators[0].Process() * Adsr.Process();
        output[i * 2] = sample; // left
        output[i * 2 + 1] = sample; // right
    }
}