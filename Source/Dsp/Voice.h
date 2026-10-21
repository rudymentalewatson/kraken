#pragma once
#include "Common.h"
#include "Envelope.h"
#include "Filters.h"
#include "Oscillators.h"
#include "Params.h"

namespace kraken
{

// Values shared by all voices (set by the engine from MIDI).
struct VoiceGlobals
{
    float sampleRate = 44100.f;
    float pitchBendSemis = 0.f;   // already scaled by the bend range
    float modWheel = 0.f;         // 0..1
};

// One voice = 16 oscillators in four groups of four:
//   Analog (2 saw + 2 pulse), FM (4 operators), Wave (4 morphing wavetables), Super (4 detuned saws)
// followed by a high-pass and a low-pass filter, and an amplifier.
class Voice
{
public:
    static constexpr int kChunk = 16;

    bool active = false;
    bool gateHeld = false;      // key is down
    bool sustained = false;     // key released but pedal is down
    int note = -1;
    uint64_t age = 0;

    float pressure = 0.f;       // aftertouch for this voice, 0..1

    void seed (uint32_t s) { rng.seed (s); }

    void noteOn (int n, float velocity, uint64_t newAge)
    {
        note = n;
        vel = velocity;
        age = newAge;
        active = true;
        gateHeld = true;
        sustained = false;

        for (int i = 0; i < 4; ++i)
        {
            phAnalog[i] = rng.uni();
            phSuper[i]  = rng.uni();
            phWave[i]   = rng.uni();
            phFm[i]     = 0.f;
        }
        fbY1 = fbY2 = 0.f;
        lfoPhase = rng.uni();
        fmEnv = 1.f;

        ampEnv.gateOn();
        fltEnv.gateOn();
    }

    void noteOff()
    {
        gateHeld = false;
        sustained = false;
        ampEnv.gateOff();
        fltEnv.gateOff();
    }

    void kill()
    {
        active = false;
        gateHeld = false;
        sustained = false;
        ampEnv.kill();
        fltEnv.kill();
        hpf.reset();
        lpf.reset();
    }

    float envelopeLevel() const { return ampEnv.level; }

    // Adds this voice (mono) into out[0..n).
    void render (float* out, int n, const Params& P, const VoiceGlobals& G)
    {
        if (! active) return;

        const float sr = G.sampleRate;
        int done = 0;

        while (done < n)
        {
            const int m = std::min (kChunk, n - done);

            // ---------- control-rate section (once per chunk) ----------
            ampEnv.set (P[AmpA], P[AmpD], P[AmpS], P[AmpR], sr);
            fltEnv.set (P[FltA], P[FltD], P[FltS], P[FltR], sr);

            lfoPhase = wrap01 (lfoPhase + P[LfoRate] * (float) m / sr);
            const float lfo = std::sin (kTwoPi * lfoPhase);

            // Analog-style drift: slow random wandering of pitch and cutoff.
            const float driftStep = (float) m / (sr * 1.5f);
            if (rng.uni() < 0.002f * (float) m) driftTarget = rng.bi();
            drift += (driftTarget - drift) * driftStep;

            const float pressureStep = 1.f - std::exp (-(float) m / (sr * 0.05f));
            pressureSmooth += (pressure - pressureSmooth) * pressureStep;

            const float vibDepth = clampf (P[LfoPitch] + G.modWheel, 0.f, 1.f) * 0.7f;   // semitones
            const float semis = (float) note + G.pitchBendSemis + P[Drift] * drift * 0.08f + vibDepth * lfo;
            const float f0 = 440.f * std::exp2 ((semis - 69.f) / 12.f);

            // filters
            const float envNow = fltEnv.level;
            float oct = P[FltEnvAmt] * 6.f * envNow
                      + P[LfoFilter] * 2.f * lfo
                      + P[AftFilter] * 3.f * pressureSmooth
                      + P[FltVel] * (vel - 0.5f) * 3.f
                      + P[Drift] * drift * 0.15f
                      + P[FltKeyTrack] * ((float) note - 60.f) / 12.f;
            const float lpfHz = P[LpfCut] * std::exp2 (oct);
            lpf.set (lpfHz, P[LpfRes], sr);
            hpf.set (P[HpfCut], P[HpfRes], sr);

            // oscillator increments
            const float detA = P[AnDetune];
            const float incA[4] = {
                f0 * centsToRatio (-detA) / sr,
                f0 * centsToRatio ( detA) / sr,
                f0 * centsToRatio (-detA * 0.5f) / sr,
                f0 * centsToRatio ( detA * 0.5f) / sr };

            const float pw = clampf (P[AnPW] + P[AnPWM] * 0.4f * lfo, 0.06f, 0.94f);
            const float pm = P[AnPulseMix];

            const float ssd = P[SsDetune] * 45.f;
            const float incS[4] = {
                f0 * centsToRatio (-ssd) / sr,
                f0 * centsToRatio (-ssd * 0.33f) / sr,
                f0 * centsToRatio ( ssd * 0.33f) / sr,
                f0 * centsToRatio ( ssd) / sr };

            const WaveTables& wt = WaveTables::get();
            int lvlA[4], lvlS[4];
            for (int i = 0; i < 4; ++i)
            {
                lvlA[i] = wt.levelFor (incA[i] * sr, sr);
                lvlS[i] = wt.levelFor (incS[i] * sr, sr);
            }
            const float wd = P[WtDetune];
            const float wOff[4] = { -1.f, -0.4f, 0.4f, 1.f };
            float incW[4];
            int lvlW[4];
            for (int i = 0; i < 4; ++i)
            {
                const float fw = f0 * centsToRatio (wd * wOff[i]);
                incW[i] = fw / sr;
                lvlW[i] = wt.levelFor (fw, sr);
            }
            const float morph = P[WtMorph];

            // FM set-up
            const int algo = (int) (P[FmAlgo] + 0.5f);
            const float ratio[4] = {
                kFmRatios[(int) clampf (P[FmRatio1] + 0.5f, 0.f, 11.f)],
                kFmRatios[(int) clampf (P[FmRatio2] + 0.5f, 0.f, 11.f)],
                kFmRatios[(int) clampf (P[FmRatio3] + 0.5f, 0.f, 11.f)],
                kFmRatios[(int) clampf (P[FmRatio4] + 0.5f, 0.f, 11.f)] };
            float incF[4];
            for (int i = 0; i < 4; ++i) incF[i] = std::min (f0 * ratio[i] / sr, 0.45f);
            const float lvl[4] = { P[FmLvl1], P[FmLvl2], P[FmLvl3], P[FmLvl4] };
            const float fbAmt = P[FmFeedback] * 1.5f;
            const float fmDecayCoef = std::exp (-4.6f / std::max (1.f, P[FmEnvDecay] * sr));
            const float fmAmt = P[FmEnvAmt];

            const float gAn = P[LvlAnalog], gFm = P[LvlFM], gWt = P[LvlWave], gSs = P[LvlSuper];
            const float velGain = 0.35f + 0.65f * vel;

            // ---------- audio-rate section ----------
            for (int s = 0; s < m; ++s)
            {
                const float amp = ampEnv.process();
                fltEnv.process();
                fmEnv *= fmDecayCoef;

                float mix = 0.f;

                // --- Analog group ---
                if (gAn > 1e-4f)
                {
                    for (int i = 0; i < 4; ++i) phAnalog[i] = wrap01 (phAnalog[i] + incA[i]);
                    // Saws come from band-limited tables (no aliasing). A pulse is the
                    // difference of two saws a "width" apart, so PWM stays clean too.
                    const int ks = WaveTables::kSawShape;
                    const float saw = 0.5f * (wt.readShape (ks, phAnalog[0], lvlA[0])
                                            + wt.readShape (ks, phAnalog[1], lvlA[1]));
                    const float pulse = 0.5f * (wt.readShape (ks, phAnalog[2], lvlA[2])
                                              - wt.readShape (ks, wrap01 (phAnalog[2] + pw), lvlA[2])
                                              + wt.readShape (ks, phAnalog[3], lvlA[3])
                                              - wt.readShape (ks, wrap01 (phAnalog[3] + pw), lvlA[3]));
                    mix += gAn * ((1.f - pm) * saw + pm * pulse);
                }

                // --- FM group (4 operators, phase modulation) ---
                if (gFm > 1e-4f)
                {
                    for (int i = 0; i < 4; ++i) phFm[i] = wrap01 (phFm[i] + incF[i]);

                    // The modulation index shrinks over time: bright attack, softer tail.
                    const float idxScale = 8.f * ((1.f - fmAmt) + fmAmt * fmEnv);

                    const float fb = 0.5f * (fbY1 + fbY2) * fbAmt;
                    float y1, y2, y3, y4, fmOut;

                    switch (algo)
                    {
                        case 0: // 4 -> 3 -> 2 -> 1
                            y4 = std::sin (kTwoPi * phFm[3] + fb);
                            y3 = std::sin (kTwoPi * phFm[2] + y4 * lvl[3] * idxScale);
                            y2 = std::sin (kTwoPi * phFm[1] + y3 * lvl[2] * idxScale);
                            y1 = std::sin (kTwoPi * phFm[0] + y2 * lvl[1] * idxScale);
                            fmOut = y1 * lvl[0];
                            break;
                        case 1: // (4 + 3) -> 2 -> 1
                            y4 = std::sin (kTwoPi * phFm[3] + fb);
                            y3 = std::sin (kTwoPi * phFm[2]);
                            y2 = std::sin (kTwoPi * phFm[1] + (y4 * lvl[3] + y3 * lvl[2]) * idxScale * 0.6f);
                            y1 = std::sin (kTwoPi * phFm[0] + y2 * lvl[1] * idxScale);
                            fmOut = y1 * lvl[0];
                            break;
                        case 2: // 4 -> 3 and 2 -> 1, two carriers
                            y4 = std::sin (kTwoPi * phFm[3] + fb);
                            y3 = std::sin (kTwoPi * phFm[2] + y4 * lvl[3] * idxScale);
                            y2 = std::sin (kTwoPi * phFm[1]);
                            y1 = std::sin (kTwoPi * phFm[0] + y2 * lvl[1] * idxScale);
                            fmOut = 0.7f * (y1 * lvl[0] + y3 * lvl[2]);
                            break;
                        case 3: // 4, 3, 2 -> 1
                            y4 = std::sin (kTwoPi * phFm[3] + fb);
                            y3 = std::sin (kTwoPi * phFm[2]);
                            y2 = std::sin (kTwoPi * phFm[1]);
                            y1 = std::sin (kTwoPi * phFm[0] + (y2 * lvl[1] + y3 * lvl[2] + y4 * lvl[3]) * idxScale * 0.5f);
                            fmOut = y1 * lvl[0];
                            break;
                        default: // four carriers (organ-like stack)
                            y4 = std::sin (kTwoPi * phFm[3] + fb);
                            y3 = std::sin (kTwoPi * phFm[2]);
                            y2 = std::sin (kTwoPi * phFm[1]);
                            y1 = std::sin (kTwoPi * phFm[0]);
                            fmOut = 0.5f * (y1 * lvl[0] + y2 * lvl[1] + y3 * lvl[2] + y4 * lvl[3]);
                            break;
                    }
                    fbY2 = fbY1;
                    fbY1 = y4;
                    mix += gFm * 1.0f * fmOut;
                }

                // --- Wave group (morphing wavetables) ---
                if (gWt > 1e-4f)
                {
                    float w = 0.f;
                    for (int i = 0; i < 4; ++i)
                    {
                        phWave[i] = wrap01 (phWave[i] + incW[i]);
                        w += wt.read (phWave[i], morph, lvlW[i]);
                    }
                    mix += gWt * 0.25f * w;
                }

                // --- Super group (4 detuned saws) ---
                if (gSs > 1e-4f)
                {
                    float w = 0.f;
                    for (int i = 0; i < 4; ++i)
                    {
                        phSuper[i] = wrap01 (phSuper[i] + incS[i]);
                        w += wt.readShape (WaveTables::kSawShape, phSuper[i], lvlS[i]);
                    }
                    mix += gSs * 0.25f * w;
                }

                // --- filters and amplifier ---
                mix *= 0.28f;
                const float h = hpf.process (mix).high;
                const float l = lpf.process (softClip (h * 1.2f)).low;
                out[done + s] += l * amp * velGain;

                if (ampEnv.isIdle())
                {
                    active = false;
                    return;
                }
            }
            done += m;
        }
    }

private:
    float vel = 0.8f;
    float phAnalog[4] {}, phSuper[4] {}, phWave[4] {}, phFm[4] {};
    float fbY1 = 0.f, fbY2 = 0.f;
    float lfoPhase = 0.f;
    float drift = 0.f, driftTarget = 0.f;
    float pressureSmooth = 0.f;
    float fmEnv = 1.f;
    Adsr ampEnv, fltEnv;
    Svf hpf, lpf;
    Rng rng;
};

} // namespace kraken
