// Offline test: renders every factory preset to a WAV file and checks the audio
// for problems (NaN, infinities, clipping, silence, CPU time).
// Usage: render_test <output-folder>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "../Source/Dsp/Engine.h"
#include "../Source/Dsp/Presets.h"

using namespace kraken;

static void writeWav (const std::string& path, const std::vector<float>& L, const std::vector<float>& R, int sr)
{
    FILE* f = std::fopen (path.c_str(), "wb");
    if (! f) { std::perror ("open"); std::exit (2); }
    const uint32_t n = (uint32_t) L.size();
    const uint32_t dataBytes = n * 4;
    auto w32 = [&] (uint32_t v) { std::fwrite (&v, 4, 1, f); };
    auto w16 = [&] (uint16_t v) { std::fwrite (&v, 2, 1, f); };
    std::fwrite ("RIFF", 1, 4, f); w32 (36 + dataBytes);
    std::fwrite ("WAVEfmt ", 1, 8, f); w32 (16); w16 (1); w16 (2); w32 ((uint32_t) sr); w32 ((uint32_t) sr * 4); w16 (4); w16 (16);
    std::fwrite ("data", 1, 4, f); w32 (dataBytes);
    for (uint32_t i = 0; i < n; ++i)
    {
        const int16_t a = (int16_t) std::lrintf (clampf (L[i], -1.f, 1.f) * 32767.f);
        const int16_t b = (int16_t) std::lrintf (clampf (R[i], -1.f, 1.f) * 32767.f);
        std::fwrite (&a, 2, 1, f);
        std::fwrite (&b, 2, 1, f);
    }
    std::fclose (f);
}

struct Stats { float peak = 0.f, rms = 0.f; bool bad = false; };

static Stats analyse (const std::vector<float>& L, const std::vector<float>& R)
{
    Stats s;
    double sum = 0.0;
    for (size_t i = 0; i < L.size(); ++i)
    {
        if (! std::isfinite (L[i]) || ! std::isfinite (R[i])) { s.bad = true; continue; }
        s.peak = std::max (s.peak, std::max (std::fabs (L[i]), std::fabs (R[i])));
        sum += (double) L[i] * L[i] + (double) R[i] * R[i];
    }
    s.rms = (float) std::sqrt (sum / (double) (2 * L.size()));
    return s;
}

int main (int argc, char** argv)
{
    const std::string outDir = argc > 1 ? argv[1] : ".";
    const int sr = 44100;
    const int block = 256;      // odd block sizes are a good stress test too

    int problems = 0;
    SynthEngine engine;
    engine.prepare (sr, block);

    std::printf ("%-18s %8s %8s %10s %s\n", "preset", "peak", "rms", "x-realtime", "status");

    const auto& presets = factoryPresets();
    for (int p = 0; p < (int) presets.size(); ++p)
    {
        engine.prepare (sr, block);                 // fresh state for each preset
        engine.setParams (presetParams (p));

        const int holdSamples = sr * 5, tailSamples = sr * 4;
        const int total = holdSamples + tailSamples;
        std::vector<float> L ((size_t) total), R ((size_t) total);

        const int notes[4] = { 48, 52, 55, 59 };    // C3 E3 G3 B3
        for (int n : notes) engine.noteOn (n, 0.8f);

        const auto t0 = std::chrono::steady_clock::now();
        int pos = 0;
        bool released = false;
        while (pos < total)
        {
            if (! released && pos >= holdSamples)
            {
                for (int n : notes) engine.noteOff (n);
                released = true;
            }
            const int m = std::min (block, total - pos);
            engine.render (L.data() + pos, R.data() + pos, m);
            pos += m;
        }
        const double secs = std::chrono::duration<double> (std::chrono::steady_clock::now() - t0).count();

        const Stats st = analyse (L, R);
        const double speed = ((double) total / sr) / secs;

        const char* status = "ok";
        if (st.bad)            { status = "NaN/Inf!";  ++problems; }
        else if (st.peak >= 0.999f) { status = "CLIPPING"; ++problems; }
        else if (st.rms < 0.005f)   { status = "TOO QUIET"; ++problems; }

        std::printf ("%-18s %8.3f %8.3f %10.1f %s\n", presets[(size_t) p].name, st.peak, st.rms, speed, status);

        std::string name = presets[(size_t) p].name;
        for (auto& c : name) if (c == ' ') c = '_';
        writeWav (outDir + "/" + name + ".wav", L, R, sr);
    }

    // --- stress test: more notes than voices, sustain pedal, pressure, bend, silence after ---
    {
        engine.prepare (sr, block);
        engine.setParams (presetParams (0));
        const int total = sr * 6;
        std::vector<float> L ((size_t) total), R ((size_t) total);
        int pos = 0;
        while (pos < total)
        {
            if (pos == 0)
                for (int n = 36; n < 36 + 14; ++n) engine.noteOn (n, 1.0f);    // 14 notes on 8 voices
            if (pos == sr * 1)  { engine.setChannelPressure (1.0f); engine.setPitchBend (0.5f); engine.setModWheel (1.0f); }
            if (pos == sr * 2)  { engine.setSustainPedal (true); for (int n = 36; n < 50; ++n) engine.noteOff (n); }
            if (pos == sr * 3)  { engine.setSustainPedal (false); }
            const int m = std::min (block, total - pos);
            engine.render (L.data() + pos, R.data() + pos, m);
            pos += m;
        }
        const Stats st = analyse (L, R);
        std::printf ("\nstress test: peak %.3f rms %.3f %s, voices still active at end: %d\n",
                     st.peak, st.rms, st.bad ? "NaN/Inf!" : "ok", engine.activeVoices());
        if (st.bad) ++problems;
        writeWav (outDir + "/_stress.wav", L, R, sr);
    }

    // --- extreme parameter test: everything at min, then max ---
    for (int extreme = 0; extreme < 2; ++extreme)
    {
        engine.prepare (sr, block);
        Params p = defaultParams();
        for (int i = 0; i < NumParams; ++i)
            p[(size_t) i] = extreme ? kParamDefs[i].maxV : kParamDefs[i].minV;
        p[LvlAnalog] = p[LvlFM] = p[LvlWave] = p[LvlSuper] = 1.f;
        p[MasterVol] = 1.f;
        engine.setParams (p);
        std::vector<float> L ((size_t) sr * 2), R ((size_t) sr * 2);
        for (int n : { 24, 60, 96, 108 }) engine.noteOn (n, 1.0f);
        engine.render (L.data(), R.data(), (int) L.size());
        const Stats st = analyse (L, R);
        std::printf ("extreme %s: peak %.3f %s\n", extreme ? "max" : "min", st.peak, st.bad ? "NaN/Inf!" : "ok");
        if (st.bad) ++problems;
    }

    // --- aliasing probe: a wide-open saw / wavetable / supersaw on very high notes ---
    for (int group = 0; group < 3; ++group)
    {
        engine.prepare (sr, block);
        Params p = defaultParams();
        p[LvlAnalog] = group == 0 ? 1.f : 0.f;
        p[LvlWave]   = group == 1 ? 1.f : 0.f;
        p[LvlSuper]  = group == 2 ? 1.f : 0.f;
        p[LvlFM] = 0.f;
        p[AnPulseMix] = 0.f; p[AnDetune] = 0.f; p[WtDetune] = 0.f; p[WtMorph] = 1.f; p[SsDetune] = 0.f;
        p[LpfCut] = 16000.f; p[LpfRes] = 0.f; p[HpfCut] = 20.f; p[FltEnvAmt] = 0.f; p[FltKeyTrack] = 0.f; p[FltVel] = 0.f;
        p[Drift] = 0.f; p[LfoPitch] = 0.f; p[LfoFilter] = 0.f;
        p[AmpA] = 0.002f; p[AmpS] = 1.f;
        p[ChorusMix] = 0.f; p[DelayMix] = 0.f; p[RevMix] = 0.f;
        engine.setParams (p);
        std::vector<float> L ((size_t) sr * 2), R ((size_t) sr * 2);
        engine.noteOn (96, 1.0f);      // C7, about 2093 Hz
        engine.render (L.data(), R.data(), (int) L.size());
        const char* names[3] = { "analog", "wave", "super" };
        writeWav (outDir + "/_alias_" + names[group] + "_C7.wav", L, R, sr);
    }

    std::printf ("\n%s (%d problem%s)\n", problems == 0 ? "ALL CHECKS PASSED" : "PROBLEMS FOUND", problems, problems == 1 ? "" : "s");
    return problems == 0 ? 0 : 1;
}
