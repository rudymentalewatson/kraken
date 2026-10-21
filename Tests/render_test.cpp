// Offline test: renders every factory preset to a WAV file and checks the audio
// for problems (NaN, infinities, clipping, silence, CPU time).
// Usage: render_test <output-folder> [cal]
//   with "cal" it also prints one machine-readable line per preset (used to balance loudness).
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
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

struct Stats { float peak = 0.f, rms = 0.f, peakFirst3s = 0.f; bool bad = false; };

static Stats analyse (const std::vector<float>& L, const std::vector<float>& R, int sr)
{
    Stats s;
    double sum = 0.0;
    for (size_t i = 0; i < L.size(); ++i)
    {
        if (! std::isfinite (L[i]) || ! std::isfinite (R[i])) { s.bad = true; continue; }
        const float a = std::max (std::fabs (L[i]), std::fabs (R[i]));
        s.peak = std::max (s.peak, a);
        if (i < (size_t) (3 * sr)) s.peakFirst3s = std::max (s.peakFirst3s, a);
        sum += (double) L[i] * L[i] + (double) R[i] * R[i];
    }
    s.rms = (float) std::sqrt (sum / (double) (2 * L.size()));
    return s;
}

// Which notes to play, depending on the family of sound.
static std::vector<int> notesFor (const std::string& category)
{
    if (category == "Bass")  return { 36, 43 };            // C2 G2
    if (category == "Leads") return { 72 };                // C5
    if (category == "Bells") return { 60, 67 };            // C4 G4
    return { 48, 52, 55, 59 };                             // C3 E3 G3 B3
}

int main (int argc, char** argv)
{
    const std::string outDir = argc > 1 ? argv[1] : ".";
    const bool wantWav = argc > 2 && std::string (argv[2]) == "wav";
    const bool calibrate = argc > 2 && std::strcmp (argv[2], "cal") == 0;
    const int sr = 44100;
    const int block = 256;

    int problems = 0;
    if (presetErrors() != 0)
    {
        std::printf ("PRESET BANK ERRORS: %d (unknown parameter names, unknown base preset or value out of range)\n", presetErrors());
        ++problems;
    }
    SynthEngine engine;
    engine.prepare (sr, block);

    if (! calibrate)
        std::printf ("%-18s %-11s %6s %6s %9s %s\n", "preset", "category", "peak", "rms", "x-realtime", "status");

    const auto& presets = factoryPresets();

    // presets must all be different from each other
    {
        std::set<std::string> seen;
        for (int p = 0; p < (int) presets.size(); ++p)
        {
            const Params v = presetParams (p);
            std::string key;
            for (float x : v) key += std::to_string (x) + ",";
            if (! seen.insert (key).second) { std::printf ("DUPLICATE preset: %s\n", presets[(size_t) p].name.c_str()); ++problems; }
        }
    }

    for (int p = 0; p < (int) presets.size(); ++p)
    {
        engine.prepare (sr, block);
        engine.setParams (presetParams (p));

        const int holdSamples = sr * 6, tailSamples = sr * 5;
        const int total = holdSamples + tailSamples;
        std::vector<float> L ((size_t) total), R ((size_t) total);

        const auto notes = notesFor (presets[(size_t) p].category);
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

        const Stats st = analyse (L, R, sr);
        const double speed = ((double) total / sr) / secs;

        const char* status = "ok";
        if (st.bad)                    { status = "NaN/Inf!";  ++problems; }
        else if (st.peak >= 0.999f)    { status = "CLIPPING";  ++problems; }
        else if (st.peak < 0.08f)      { status = "TOO QUIET"; ++problems; }

        if (calibrate)
            std::printf ("CAL|%s|%.5f\n", presets[(size_t) p].name.c_str(), st.peak);
        else
            std::printf ("%-18s %-11s %6.3f %6.3f %9.1f %s\n", presets[(size_t) p].name.c_str(), presets[(size_t) p].category.c_str(),
                         st.peak, st.rms, speed, status);

        std::string name = presets[(size_t) p].name;
        for (auto& c : name) if (c == ' ') c = '_';
        if (wantWav) writeWav (outDir + "/" + name + ".wav", L, R, sr);
    }

    if (calibrate)
        return 0;

    // --- stress test: more notes than voices, sustain pedal, pressure, bend ---
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
        const Stats st = analyse (L, R, sr);
        std::printf ("\nstress test: peak %.3f rms %.3f %s, voices still active at end: %d\n",
                     st.peak, st.rms, st.bad ? "NaN/Inf!" : "ok", engine.activeVoices());
        if (st.bad) ++problems;
        if (wantWav) writeWav (outDir + "/_stress.wav", L, R, sr);
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
        const Stats st = analyse (L, R, sr);
        std::printf ("extreme %s: peak %.3f %s\n", extreme ? "max" : "min", st.peak, st.bad ? "NaN/Inf!" : "ok");
        if (st.bad) ++problems;
    }

    std::printf ("\n%s (%d problem%s)\n", problems == 0 ? "ALL CHECKS PASSED" : "PROBLEMS FOUND", problems, problems == 1 ? "" : "s");
    return problems == 0 ? 0 : 1;
}
