#include "../headers/voice.hpp"

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
