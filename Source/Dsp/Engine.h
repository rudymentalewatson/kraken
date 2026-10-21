#pragma once
#include "Common.h"
#include "Effects.h"
#include "Params.h"
#include "Voice.h"

namespace kraken
{

// The whole instrument: 8 voices + effects. No dependency on any plugin framework,
// so it can be tested offline and reused anywhere.
class SynthEngine
{
public:
    static constexpr int kNumVoices = 8;

    void prepare (double sr, int maxBlock)
    {
        sampleRate = (float) sr;
        globals.sampleRate = sampleRate;
        scratch.assign ((size_t) std::max (maxBlock, 64), 0.f);
        for (int i = 0; i < kNumVoices; ++i)
        {
            voices[i].seed (1000u + 7919u * (uint32_t) i);
            voices[i].kill();
        }
        ensemble.prepare (sampleRate);
        delay.prepare (sampleRate);
        reverb.prepare (sampleRate);
        params = defaultParams();
        masterSmooth = params[MasterVol];
        ageCounter = 0;
    }

    void setParams (const Params& p) { params = p; }
    void setParam (int id, float v) { params[(size_t) id] = v; }

    // ---- MIDI-style control ----
    void noteOn (int note, float velocity)
    {
        Voice* v = nullptr;

        // retrigger the same note if it is already sounding
        for (auto& vc : voices)
            if (vc.active && vc.note == note) { v = &vc; break; }

        if (v == nullptr)
            for (auto& vc : voices)
                if (! vc.active) { v = &vc; break; }

        if (v == nullptr)
        {
            // steal: prefer releasing voices (quietest first), otherwise the oldest
            float best = 1e9f;
            for (auto& vc : voices)
                if (! vc.gateHeld && ! vc.sustained && vc.envelopeLevel() < best)
                {
                    best = vc.envelopeLevel();
                    v = &vc;
                }
            if (v == nullptr)
            {
                uint64_t oldest = ~0ull;
                for (auto& vc : voices)
                    if (vc.age < oldest) { oldest = vc.age; v = &vc; }
            }
        }

        v->noteOn (note, velocity, ++ageCounter);
        v->pressure = std::max (channelPressure, polyPressure[note & 127]);
    }

    void noteOff (int note)
    {
        for (auto& vc : voices)
            if (vc.active && vc.gateHeld && vc.note == note)
            {
                if (sustainPedal) { vc.sustained = true; vc.gateHeld = false; }
                else vc.noteOff();
            }
    }

    void setSustainPedal (bool down)
    {
        sustainPedal = down;
        if (! down)
            for (auto& vc : voices)
                if (vc.active && vc.sustained)
                    vc.noteOff();
    }

    void allNotesOff()
    {
        for (auto& vc : voices)
            if (vc.active) vc.noteOff();
        sustainPedal = false;
    }

    void setPitchBend (float norm /* -1..1 */)    { globals.pitchBendSemis = norm * 2.f; }
    void setModWheel (float v /* 0..1 */)         { globals.modWheel = v; }

    void setChannelPressure (float v)
    {
        channelPressure = v;
        for (auto& vc : voices)
            vc.pressure = std::max (channelPressure, polyPressure[vc.note & 127]);
    }

    void setPolyPressure (int note, float v)
    {
        polyPressure[note & 127] = v;
        for (auto& vc : voices)
            if (vc.note == note)
                vc.pressure = std::max (channelPressure, v);
    }

    int activeVoices() const
    {
        int n = 0;
        for (auto& v : voices) if (v.active) ++n;
        return n;
    }

    // Renders n samples into L and R (overwrites them).
    void render (float* L, float* R, int n)
    {
        int done = 0;
        while (done < n)
        {
            const int m = std::min ((int) scratch.size(), n - done);
            std::fill (scratch.begin(), scratch.begin() + m, 0.f);

            for (auto& v : voices)
                v.render (scratch.data(), m, params, globals);

            for (int i = 0; i < m; ++i)
            {
                masterSmooth += (params[MasterVol] - masterSmooth) * 0.002f;
                const float x = scratch[(size_t) i] * masterSmooth;

                float l, r;
                ensemble.process (x, params[ChorusMix], params[ChorusDepth], params[ChorusRate], l, r);
                delay.process (l, r, params[DelayMix], params[DelayTime], params[DelayFb]);
                reverb.process (l, r, params[RevMix], params[RevSize], params[RevDamp]);

                L[done + i] = softClip (l);
                R[done + i] = softClip (r);
            }
            done += m;
        }
    }

private:
    float sampleRate = 44100.f;
    Params params = defaultParams();
    VoiceGlobals globals;
    Voice voices[kNumVoices];
    Ensemble ensemble;
    StereoDelay delay;
    Reverb reverb;
    std::vector<float> scratch;
    float masterSmooth = 0.7f;
    float channelPressure = 0.f;
    float polyPressure[128] {};
    bool sustainPedal = false;
    uint64_t ageCounter = 0;
};

} // namespace kraken
