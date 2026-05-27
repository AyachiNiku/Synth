#include "../headers/voice.hpp"

void Voice::CycleWaveform(bool right) {
    for (auto& osc : _oscillators) {
        osc.CycleWaveform(right);
    }
}

void Voice::CycleWaveform(int oscillatorId, bool right) {
    _oscillators[oscillatorId].CycleWaveform(right);
}

// puts together the result signal of all three oscillators
float Voice::Process() {
    if (!_isActive) return 0.0f;

    int activeCount = 0;
    float sample = 0.0f;
    for (auto& osc : _oscillators) {
        if (osc.IsActive()) {
            activeCount++;
            sample += osc.Process();
        }
    }

    if (activeCount > 0) {
        sample /= static_cast<float>(activeCount);
    }

    float envelope = _adsr.Process();

    if (_adsr.GetState() == ADSRState::Idle) {
        _isActive = false;
    }

    return sample * envelope * _velocity;
}

// sets the note of a voice
void Voice::NoteOn(Note::Id note, float velocity) {
    _note = note;
    _isNoteOn = true;
    _isActive = true;
    _velocity = velocity / 127.0f;  // conversion from MIDI's 0-127 format to 0-1
    for (auto& osc : _oscillators) {
        osc.SetFrequency(Note::ToFrequency(note));
    }
    _adsr.NoteOn();
}

void Voice::NoteOff(Note::Id note) {
    _isNoteOn = false;
    _adsr.NoteOff();
}

Note::Id Voice::GetNote() const {
    return _note;
}

bool Voice::IsDisabled() const {
    return _isDisabled;
}

bool Voice::IsActive() const {
    return _isActive;
}

bool Voice::IsNoteOn() const {
    return _isNoteOn;
}

void Voice::Disable() {
    _isNoteOn = false;
    _isActive = false;
    _adsr.NoteOff();
    _isDisabled = true;
}

void Voice::Enable() {
    _isDisabled = false;
}