#pragma once
#include <vector>
#include "Params.h"

namespace kraken
{

struct PresetEntry
{
    const char* id;
    float value;
};

struct Preset
{
    const char* name;
    std::vector<PresetEntry> entries;   // only what differs from the defaults
};

inline const std::vector<Preset>& factoryPresets()
{
    static const std::vector<Preset> list = {
        { "Neon Rain", {   // wide, slow-breathing analog pad (the cinematic one)
            { "lvlAnalog", 1.0f }, { "lvlFM", 0.10f }, { "lvlWave", 0.0f }, { "lvlSuper", 0.35f },
            { "anPulse", 0.25f }, { "anDetune", 14.f }, { "anPWM", 0.5f },
            { "lpfCut", 1100.f }, { "lpfRes", 0.30f }, { "fltEnv", 0.55f }, { "fltKey", 0.6f },
            { "fltA", 1.1f }, { "fltD", 2.5f }, { "fltS", 0.45f }, { "fltR", 2.2f },
            { "ampA", 0.9f }, { "ampD", 2.0f }, { "ampS", 0.85f }, { "ampR", 2.4f },
            { "aftFilter", 0.7f }, { "lfoPitch", 0.08f }, { "drift", 0.5f },
            { "chMix", 0.6f }, { "chDepth", 0.55f }, { "dlMix", 0.2f }, { "rvMix", 0.45f }, { "rvSize", 0.9f } } },

        { "Off-World Brass", {   // big swelling brass, opens with the key pressure
            { "lvlAnalog", 1.0f }, { "lvlFM", 0.0f }, { "lvlWave", 0.0f }, { "lvlSuper", 0.5f },
            { "anPulse", 0.2f }, { "anDetune", 10.f },
            { "hpfCut", 120.f }, { "lpfCut", 700.f }, { "lpfRes", 0.2f }, { "fltEnv", 0.8f },
            { "fltA", 0.35f }, { "fltD", 1.2f }, { "fltS", 0.55f }, { "fltR", 0.9f },
            { "ampA", 0.25f }, { "ampD", 1.0f }, { "ampS", 0.9f }, { "ampR", 0.9f },
            { "aftFilter", 0.9f }, { "lfoPitch", 0.1f },
            { "chMix", 0.45f }, { "dlMix", 0.1f }, { "rvMix", 0.3f } } },

        { "Glass Bells", {   // FM bells, long tail
            { "lvlAnalog", 0.0f }, { "lvlFM", 1.0f }, { "lvlWave", 0.15f }, { "lvlSuper", 0.0f },
            { "wtMorph", 0.7f },
            { "fmAlgo", 2.f }, { "fmRatio1", 3.f }, { "fmRatio2", 9.f }, { "fmRatio3", 5.f }, { "fmRatio4", 11.f },
            { "fmLvl1", 1.0f }, { "fmLvl2", 0.55f }, { "fmLvl3", 0.7f }, { "fmLvl4", 0.4f },
            { "fmFb", 0.0f }, { "fmEnv", 0.9f }, { "fmDecay", 2.5f },
            { "lpfCut", 9000.f }, { "fltEnv", 0.0f }, { "fltKey", 0.8f },
            { "ampA", 0.003f }, { "ampD", 3.5f }, { "ampS", 0.0f }, { "ampR", 2.5f },
            { "chMix", 0.35f }, { "dlMix", 0.3f }, { "rvMix", 0.4f } } },

        { "Lunar Strings", {   // soft ensemble strings
            { "lvlAnalog", 0.5f }, { "lvlFM", 0.0f }, { "lvlWave", 0.2f }, { "lvlSuper", 0.8f },
            { "wtMorph", 0.2f }, { "anPulse", 0.1f }, { "ssDetune", 0.5f },
            { "lpfCut", 3200.f }, { "fltEnv", 0.2f },
            { "ampA", 1.4f }, { "ampD", 2.0f }, { "ampS", 0.9f }, { "ampR", 2.8f },
            { "chMix", 0.8f }, { "chDepth", 0.7f }, { "rvMix", 0.5f } } },

        { "Supernova Wall", {   // huge detuned wall, slow filter bloom
            { "lvlAnalog", 0.4f }, { "lvlFM", 0.0f }, { "lvlWave", 0.3f }, { "lvlSuper", 1.0f },
            { "ssDetune", 0.9f }, { "wtMorph", 0.85f },
            { "lpfCut", 5000.f }, { "lpfRes", 0.35f }, { "fltEnv", 0.6f }, { "fltA", 2.5f }, { "fltD", 3.f },
            { "ampA", 1.8f }, { "ampR", 3.0f },
            { "chMix", 0.6f }, { "dlMix", 0.3f }, { "dlFb", 0.5f }, { "rvMix", 0.55f }, { "rvSize", 1.0f } } },

        { "Electric Keys", {   // FM electric piano
            { "lvlAnalog", 0.1f }, { "lvlFM", 1.0f }, { "lvlWave", 0.0f }, { "lvlSuper", 0.0f },
            { "fmAlgo", 2.f }, { "fmRatio1", 3.f }, { "fmRatio2", 11.f }, { "fmRatio3", 3.f }, { "fmRatio4", 3.f },
            { "fmLvl1", 1.0f }, { "fmLvl2", 0.25f }, { "fmLvl3", 0.5f }, { "fmLvl4", 0.3f },
            { "fmFb", 0.0f }, { "fmEnv", 0.8f }, { "fmDecay", 0.6f },
            { "lpfCut", 6000.f }, { "fltEnv", 0.0f }, { "fltKey", 0.7f },
            { "ampA", 0.003f }, { "ampD", 2.2f }, { "ampS", 0.0f }, { "ampR", 0.6f },
            { "chMix", 0.6f }, { "rvMix", 0.25f }, { "dlMix", 0.05f } } },

        { "Dark Drone", {   // low, moving, uneasy
            { "lvlAnalog", 1.0f }, { "lvlFM", 0.0f }, { "lvlWave", 0.4f }, { "lvlSuper", 0.0f },
            { "wtMorph", 0.45f }, { "anPulse", 0.6f }, { "anPWM", 0.8f }, { "anDetune", 18.f },
            { "lpfCut", 450.f }, { "lpfRes", 0.45f }, { "fltEnv", 0.3f }, { "lfoRate", 0.12f }, { "lfoFilter", 0.4f },
            { "drift", 0.8f },
            { "ampA", 3.5f }, { "ampD", 3.f }, { "ampS", 1.0f }, { "ampR", 5.0f },
            { "dlMix", 0.35f }, { "dlFb", 0.6f }, { "rvMix", 0.6f } } },

        { "Warm Choir", {   // vocal-like wavetable pad
            { "lvlAnalog", 0.3f }, { "lvlFM", 0.0f }, { "lvlWave", 1.0f }, { "lvlSuper", 0.0f },
            { "wtMorph", 0.33f }, { "wtDetune", 12.f },
            { "lpfCut", 2800.f }, { "fltEnv", 0.2f },
            { "ampA", 1.0f }, { "ampS", 0.9f }, { "ampR", 2.5f },
            { "chMix", 0.7f }, { "rvMix", 0.5f } } },

        { "Init Saw", {   // clean starting point for sound design
            { "lvlAnalog", 1.0f }, { "lvlFM", 0.0f }, { "lvlWave", 0.0f }, { "lvlSuper", 0.0f },
            { "anPulse", 0.0f }, { "anDetune", 6.f },
            { "lpfCut", 8000.f }, { "lpfRes", 0.1f }, { "fltEnv", 0.0f },
            { "ampA", 0.01f }, { "ampD", 0.5f }, { "ampS", 1.0f }, { "ampR", 0.3f },
            { "chMix", 0.0f }, { "dlMix", 0.0f }, { "rvMix", 0.0f } } },
    };
    return list;
}

// Defaults + the preset's changes.
inline Params presetParams (int index)
{
    Params p = defaultParams();
    const auto& list = factoryPresets();
    if (index < 0 || index >= (int) list.size())
        return p;
    for (const auto& e : list[(size_t) index].entries)
    {
        const int id = findParam (e.id);
        if (id >= 0)
            p[(size_t) id] = e.value;
    }
    return p;
}

} // namespace kraken
