#include "../headers/synth.hpp"

#include <iostream>

void Synth::CycleWaveform(bool right) {
    for (auto& voice : _voices) {
        voice.CycleWaveform(right);
    }
}

// get the chosen oscillator out of the three from the GUI and only change its waveform, not all three
void Synth::CycleWaveform(int oscillatorId, bool right) {
    for (auto& voice : _voices) {
        voice.CycleWaveform(oscillatorId, right);
    }
}

// round-robin allocation
void Synth::NoteOn(Note::Id note, float velocity) {
    // if this note is already playing, just retrigger it
    for (auto& voice : _voices) {
        if (voice.IsActive() && voice.GetNote() == note) {
            voice.NoteOn(note, velocity);
            return;
        }
    }

    // find next voice using round-robin
   while (true) {
       if (!_voices[_currentVoice].IsDisabled()) {
           _voices[_currentVoice].NoteOn(note, velocity);
           _currentVoice = (_currentVoice + 1) % _voices.size();
           break;
       }
       _currentVoice = (_currentVoice + 1) % _voices.size();
   }
}

void Synth::NoteOff(Note::Id note) {
    for (auto& voice : _voices) {
        if (voice.IsNoteOn() && voice.GetNote() == note) {
            voice.NoteOff(note);
            return;
        }
    }
}

void Synth::Run(float* output, unsigned long sampleRate) {
    // for each sample in sampleRate,
    for (unsigned long i = 0; i < sampleRate; i++) {
        float sample = 0.0f;
        int activeCount = 0;

        for (auto& voice : _voices) {
            if (!voice.IsDisabled() && voice.IsActive()) {
                sample += voice.Process();
                activeCount++;
            }
        }

        size_t enabledVoices = _voices.size() - _disabledVoices;
        if (activeCount != 0) {
            sample /= static_cast<float>(enabledVoices);
        }

        if (_delay.IsActive()) {
            sample = _delay.Process(sample);
        }

        sample = _filters[_currentFilter]->Process(sample);

        output[i * 2] = sample; // left
        output[i * 2 + 1] = sample; // right
    }
}

void Synth::AdjustVoiceAmount(bool down) {
    if (down && _disabledVoices < 7) { // max 7, always keep at least 1
        for (auto& voice : _voices) {
            if (!voice.IsNoteOn() && !voice.IsDisabled()) {
                voice.Disable();
                _disabledVoices++;
                break;
            }
        }
    } else if (!down && _disabledVoices > 0) {
        for (auto& voice : _voices) {
            if (voice.IsDisabled()) {
                voice.Enable();
                _disabledVoices--;
                break;
            }
        }
    }
}

void Synth::CycleFilter(bool right) {
    if (right) {
        _currentFilter = (_currentFilter + 1) % _filters.size();
    } else {
        _currentFilter = (_currentFilter - 1 + _filters.size()) % _filters.size();
    }
}