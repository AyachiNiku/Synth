#pragma once

#include <array>
#include <memory>

#include "voice.hpp"
#include "delay.hpp"
#include "filter.hpp"

class Synth {
public:
    Synth() {
        _filters.push_back(std::make_unique<LowPassFilter>());
        _filters.push_back(std::make_unique<HighPassFilter>());
        _filters.push_back(std::make_unique<BandPassFilter>());
    }

    void Run(float* output, unsigned long sampleRate);
    void CycleFilter(bool right);
    void NoteOn(Note::Id note, float velocity = 127.0f);
    void NoteOff(Note::Id note);
    void AdjustVoiceAmount(bool down);

    // gets the first voice in the array that's not disabled
    Voice GetFirstVoice() const;
    size_t GetVoiceCount() const { return _voices.size() - _disabledVoices; }

    void SwitchOscillatorState(size_t i) { for (auto& voice : _voices) voice.SwitchOscillatorState(i); }

    // Waveforms
    void CycleWaveform(bool right);
    void CycleWaveform(size_t oscillatorId, bool right);

    // Delay
    void SwitchDelayState() { _delay.SwitchState(); }
    void SetDelayFeedback(float feedback) { _delay.SetFeedback(feedback); }
    void SetDelayMix(float mix) { _delay.SetMix(mix); }
    void SetDelayTime(float seconds) { _delay.SetDelayTime(seconds); }

    bool IsDelayActive() const { return _delay.IsActive(); }
    float GetDelayFeedback() const { return _delay.GetFeedback(); }
    float GetDelayMix() const { return _delay.GetMix(); }
    float GetDelayTime() const { return _delay.GetDelayTime(); }

    // Filters
    void SwitchFilterState(size_t id) { _filters.at(id)->SwitchState(); }
    void SetFilterCutoff(size_t id, float cutoff) { _filters.at(id)->SetCutoff(cutoff); }
    void SetFilterQ(size_t id, float q) { _filters.at(id)->SetQ(q); }

    bool IsFilterActive(size_t id) const { return _filters.at(id)->IsActive(); }
    float GetFilterCutoff(size_t id) const { return _filters.at(id)->GetCutoff(); }
    float GetFilterQ(size_t id) const { return _filters.at(id)->GetQ(); }
    size_t GetCurrentFilter() const { return _currentFilter; }

    // ADSR
    float GetAttack() const { return _voices[0].GetAttack(); }
    float GetDecay() const { return _voices[0].GetDecay(); }
    float GetSustain() const { return _voices[0].GetSustain(); }
    float GetRelease() const { return _voices[0].GetRelease(); }

    void SetAttack(float attackTime) { for (auto& voice : _voices) voice.SetAttack(attackTime); }
    void SetDecay(float decayTime) { for (auto& voice : _voices) voice.SetDecay(decayTime); }
    void SetSustain(float sustainLevel) { for (auto& voice : _voices) voice.SetSustain(sustainLevel); }
    void SetRelease(float releaseTime) { for (auto& voice : _voices) voice.SetRelease(releaseTime); }

private:
    std::array<Voice, 8> _voices;
    size_t _disabledVoices = 0;
    size_t _currentVoice = 0;
    Delay _delay;
    std::vector<std::unique_ptr<Filter>> _filters;
    size_t _currentFilter = 0;
};
