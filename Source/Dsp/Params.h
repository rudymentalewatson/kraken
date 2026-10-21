#pragma once
// Kraken - parameter registry.
// One single list drives everything: the enum, the plugin parameters, the
// interface and the presets. To add a knob, add one line here.
#include <array>
#include <cstring>

namespace kraken
{

// X(enumName, id, displayName, section, min, max, default, centre, step, unit)
//   centre: value at the middle of the slider (0 = linear slider)
//   step:   0 = continuous, 1 = whole numbers only
#define KRAKEN_PARAMS(X)                                                                         \
    X(MasterVol,   "master",    "Volume",    "Mix",         0.f,    1.f,     0.7f,   0.f,    0.f, "")      \
    X(LvlAnalog,   "lvlAnalog", "Analog",    "Mix",         0.f,    1.f,     0.8f,   0.f,    0.f, "")      \
    X(LvlFM,       "lvlFM",     "FM",        "Mix",         0.f,    1.f,     0.25f,  0.f,    0.f, "")      \
    X(LvlWave,     "lvlWave",   "Wave",      "Mix",         0.f,    1.f,     0.2f,   0.f,    0.f, "")      \
    X(LvlSuper,    "lvlSuper",  "Super",     "Mix",         0.f,    1.f,     0.3f,   0.f,    0.f, "")      \
    X(AnPulseMix,  "anPulse",   "Pulse",     "Analog",      0.f,    1.f,     0.35f,  0.f,    0.f, "")      \
    X(AnPW,        "anPW",      "Width",     "Analog",      0.1f,   0.9f,    0.5f,   0.f,    0.f, "")      \
    X(AnPWM,       "anPWM",     "PWM",       "Analog",      0.f,    1.f,     0.3f,   0.f,    0.f, "")      \
    X(AnDetune,    "anDetune",  "Detune",    "Analog",      0.f,    50.f,    12.f,   15.f,   0.f, "ct")    \
    X(WtMorph,     "wtMorph",   "Morph",     "Wave/Super",  0.f,    1.f,     0.3f,   0.f,    0.f, "")      \
    X(WtDetune,    "wtDetune",  "Detune",    "Wave/Super",  0.f,    30.f,    8.f,    8.f,    0.f, "ct")    \
    X(SsDetune,    "ssDetune",  "Super Det", "Wave/Super",  0.f,    1.f,     0.4f,   0.f,    0.f, "")      \
    X(FmAlgo,      "fmAlgo",    "Algo",      "FM",          0.f,    4.f,     0.f,    0.f,    1.f, "")      \
    X(FmRatio1,    "fmRatio1",  "Ratio 1",   "FM",          0.f,    11.f,    3.f,    0.f,    1.f, "")      \
    X(FmRatio2,    "fmRatio2",  "Ratio 2",   "FM",          0.f,    11.f,    5.f,    0.f,    1.f, "")      \
    X(FmRatio3,    "fmRatio3",  "Ratio 3",   "FM",          0.f,    11.f,    3.f,    0.f,    1.f, "")      \
    X(FmRatio4,    "fmRatio4",  "Ratio 4",   "FM",          0.f,    11.f,    6.f,    0.f,    1.f, "")      \
    X(FmFeedback,  "fmFb",      "Feedback",  "FM",          0.f,    1.f,     0.1f,   0.f,    0.f, "")      \
    X(FmLvl1,      "fmLvl1",    "Level 1",   "FM",          0.f,    1.f,     1.0f,   0.f,    0.f, "")      \
    X(FmLvl2,      "fmLvl2",    "Level 2",   "FM",          0.f,    1.f,     0.35f,  0.f,    0.f, "")      \
    X(FmLvl3,      "fmLvl3",    "Level 3",   "FM",          0.f,    1.f,     0.25f,  0.f,    0.f, "")      \
    X(FmLvl4,      "fmLvl4",    "Level 4",   "FM",          0.f,    1.f,     0.15f,  0.f,    0.f, "")      \
    X(FmEnvAmt,    "fmEnv",     "Env Amt",   "FM",          0.f,    1.f,     0.6f,   0.f,    0.f, "")      \
    X(FmEnvDecay,  "fmDecay",   "Env Decay", "FM",          0.05f,  6.f,     1.2f,   0.8f,   0.f, "s")     \
    X(HpfCut,      "hpfCut",    "HPF Cut",   "Filter",      20.f,   4000.f,  40.f,   300.f,  0.f, "Hz")    \
    X(HpfRes,      "hpfRes",    "HPF Res",   "Filter",      0.f,    0.9f,    0.1f,   0.f,    0.f, "")      \
    X(LpfCut,      "lpfCut",    "LPF Cut",   "Filter",      60.f,   16000.f, 2200.f, 1500.f, 0.f, "Hz")    \
    X(LpfRes,      "lpfRes",    "LPF Res",   "Filter",      0.f,    0.95f,   0.25f,  0.f,    0.f, "")      \
    X(FltEnvAmt,   "fltEnv",    "Env Amt",   "Filter",      -1.f,   1.f,     0.45f,  0.f,    0.f, "")      \
    X(FltKeyTrack, "fltKey",    "Key Track", "Filter",      0.f,    1.f,     0.5f,   0.f,    0.f, "")      \
    X(FltVel,      "fltVel",    "Vel Amt",   "Filter",      0.f,    1.f,     0.3f,   0.f,    0.f, "")      \
    X(AftFilter,   "aftFilter", "Aftertouch","Filter",      0.f,    1.f,     0.5f,   0.f,    0.f, "")      \
    X(AmpA,        "ampA",      "Attack",    "Amp Env",     0.002f, 8.f,     0.6f,   0.4f,   0.f, "s")     \
    X(AmpD,        "ampD",      "Decay",     "Amp Env",     0.01f,  8.f,     1.5f,   0.8f,   0.f, "s")     \
    X(AmpS,        "ampS",      "Sustain",   "Amp Env",     0.f,    1.f,     0.8f,   0.f,    0.f, "")      \
    X(AmpR,        "ampR",      "Release",   "Amp Env",     0.02f,  10.f,    1.8f,   1.0f,   0.f, "s")     \
    X(FltA,        "fltA",      "Attack",    "Filter Env",  0.002f, 8.f,     0.8f,   0.4f,   0.f, "s")     \
    X(FltD,        "fltD",      "Decay",     "Filter Env",  0.01f,  8.f,     2.0f,   0.8f,   0.f, "s")     \
    X(FltS,        "fltS",      "Sustain",   "Filter Env",  0.f,    1.f,     0.4f,   0.f,    0.f, "")      \
    X(FltR,        "fltR",      "Release",   "Filter Env",  0.02f,  10.f,    1.8f,   1.0f,   0.f, "s")     \
    X(LfoRate,     "lfoRate",   "Rate",      "LFO",         0.05f,  12.f,    0.6f,   1.5f,   0.f, "Hz")    \
    X(LfoPitch,    "lfoPitch",  "Vibrato",   "LFO",         0.f,    1.f,     0.05f,  0.f,    0.f, "")      \
    X(LfoFilter,   "lfoFilter", "Filter",    "LFO",         0.f,    1.f,     0.1f,   0.f,    0.f, "")      \
    X(Drift,       "drift",     "Drift",     "LFO",         0.f,    1.f,     0.4f,   0.f,    0.f, "")      \
    X(ChorusMix,   "chMix",     "Mix",       "Ensemble",    0.f,    1.f,     0.5f,   0.f,    0.f, "")      \
    X(ChorusDepth, "chDepth",   "Depth",     "Ensemble",    0.f,    1.f,     0.5f,   0.f,    0.f, "")      \
    X(ChorusRate,  "chRate",    "Rate",      "Ensemble",    0.1f,   3.f,     0.6f,   0.7f,   0.f, "Hz")    \
    X(DelayMix,    "dlMix",     "Mix",       "Delay",       0.f,    1.f,     0.15f,  0.f,    0.f, "")      \
    X(DelayTime,   "dlTime",    "Time",      "Delay",       0.05f,  1.0f,    0.42f,  0.35f,  0.f, "s")     \
    X(DelayFb,     "dlFb",      "Feedback",  "Delay",       0.f,    0.9f,    0.4f,   0.f,    0.f, "")      \
    X(RevMix,      "rvMix",     "Mix",       "Reverb",      0.f,    1.f,     0.35f,  0.f,    0.f, "")      \
    X(RevSize,     "rvSize",    "Size",      "Reverb",      0.f,    1.f,     0.8f,   0.f,    0.f, "")      \
    X(RevDamp,     "rvDamp",    "Damping",   "Reverb",      0.f,    1.f,     0.4f,   0.f,    0.f, "")

enum ParamId : int
{
#define KRAKEN_ENUM(name, id, disp, section, mn, mx, df, ce, st, un) name,
    KRAKEN_PARAMS(KRAKEN_ENUM)
#undef KRAKEN_ENUM
    NumParams
};

struct ParamDef
{
    const char* id;
    const char* name;
    const char* section;
    float minV, maxV, defV;
    float centre;
    float step;
    const char* unit;
};

inline constexpr ParamDef kParamDefs[NumParams] = {
#define KRAKEN_DEF(name, id, disp, section, mn, mx, df, ce, st, un) { id, disp, section, mn, mx, df, ce, st, un },
    KRAKEN_PARAMS(KRAKEN_DEF)
#undef KRAKEN_DEF
};

// FM operator frequency ratios selectable with the "Ratio" knobs.
inline constexpr float kFmRatios[12] = { 0.25f, 0.5f, 0.75f, 1.f, 1.5f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f };

using Params = std::array<float, NumParams>;

inline Params defaultParams()
{
    Params p {};
    for (int i = 0; i < NumParams; ++i)
        p[(size_t) i] = kParamDefs[i].defV;
    return p;
}

// Returns the index of a parameter from its text id, or -1 if unknown.
inline int findParam(const char* id)
{
    for (int i = 0; i < NumParams; ++i)
        if (std::strcmp(kParamDefs[i].id, id) == 0)
            return i;
    return -1;
}

} // namespace kraken
