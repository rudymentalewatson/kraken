#pragma once
#include "Common.h"

namespace kraken
{

// Band-limited saw and pulse (PolyBLEP). phase and dt are in cycles (0..1).
inline float sawBl (float phase, float dt)
{
    return 2.f * phase - 1.f - polyBlep (phase, dt);
}

inline float pulseBl (float phase, float dt, float width)
{
    float v = phase < width ? 1.f : -1.f;
    v += polyBlep (phase, dt);
    v -= polyBlep (wrap01 (phase + 1.f - width), dt);
    return v;
}

// Four single-cycle spectra, each stored in 10 band-limited versions (mip levels).
// The "Morph" knob glides between them.
//   0 Organ  1 Choir  2 Glass  3 Saw
class WaveTables
{
public:
    static constexpr int kSize = 2048;
    static constexpr int kLevels = 10;
    static constexpr int kShapes = 4;

    static const WaveTables& get()
    {
        static const WaveTables instance;
        return instance;
    }

    // Level with the most harmonics that still stays under Nyquist.
    int levelFor (float freq, float sampleRate) const
    {
        const float maxH = 0.5f * sampleRate / std::max (freq, 1.f);
        int l = 0;
        while (l < kLevels - 1 && (float) (1024 >> l) > maxH)
            ++l;
        return l;
    }

    // One stored shape, no morphing (used for band-limited saws).
    inline float readShape (int shape, float phase, int level) const
    {
        const float pos = phase * (float) kSize;
        int i = (int) pos;
        const float f = pos - (float) i;
        i &= (kSize - 1);
        const float* a = table (shape, level);
        return a[i] + f * (a[i + 1] - a[i]);
    }

    static constexpr int kSawShape = 3;

    // phase 0..1, morph 0..1, level from levelFor()
    inline float read (float phase, float morph, int level) const
    {
        const float m = clampf (morph, 0.f, 1.f) * (float) (kShapes - 1);
        int s = (int) m;
        if (s > kShapes - 2) s = kShapes - 2;
        const float mf = m - (float) s;

        const float pos = phase * (float) kSize;
        int i = (int) pos;
        const float f = pos - (float) i;
        i &= (kSize - 1);

        const float* a = table (s, level);
        const float* b = table (s + 1, level);
        const float va = a[i] + f * (a[i + 1] - a[i]);
        const float vb = b[i] + f * (b[i + 1] - b[i]);
        return va + mf * (vb - va);
    }

private:
    std::vector<float> data;

    WaveTables()
    {
        data.assign ((size_t) kShapes * kLevels * (kSize + 1), 0.f);
        build();
    }

    const float* table (int shape, int level) const
    {
        return &data[(size_t) ((shape * kLevels + level) * (kSize + 1))];
    }
    float* tableW (int shape, int level)
    {
        return &data[(size_t) ((shape * kLevels + level) * (kSize + 1))];
    }

    static float harmonicAmp (int shape, int h)
    {
        const float x = (float) h;
        switch (shape)
        {
            case 0: // Organ: a few "drawbars"
                switch (h)
                {
                    case 1: return 1.0f;
                    case 2: return 0.6f;
                    case 3: return 0.35f;
                    case 4: return 0.5f;
                    case 6: return 0.25f;
                    case 8: return 0.2f;
                    default: return 0.f;
                }
            case 1: // Choir: vowel-like humps in the spectrum
            {
                const float a = (x - 6.f) / 2.f, b = (x - 14.f) / 3.f;
                return std::pow (x, -0.8f) * (1.f + 3.f * std::exp (-a * a) + 2.f * std::exp (-b * b));
            }
            case 2: // Glass: sparse, bell-like series
            {
                static const int fib[] = { 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610 };
                for (int f : fib)
                    if (f == h) return std::pow (x, -0.4f);
                return 0.03f / x;
            }
            default: // Saw
                return 1.f / x;
        }
    }

    void build()
    {
        // Sine lookup so building the tables stays quick at start-up.
        std::vector<float> sine (kSize);
        for (int i = 0; i < kSize; ++i)
            sine[(size_t) i] = std::sin (kTwoPi * (float) i / (float) kSize);

        for (int s = 0; s < kShapes; ++s)
        {
            float norm = 1.f;
            for (int l = 0; l < kLevels; ++l)
            {
                float* t = tableW (s, l);
                const int maxH = 1024 >> l;
                for (int h = 1; h <= maxH; ++h)
                {
                    const float a = harmonicAmp (s, h);
                    if (a == 0.f) continue;
                    for (int i = 0; i < kSize; ++i)
                        t[i] += a * sine[(size_t) ((h * i) & (kSize - 1))];
                }
                if (l == 0)
                {
                    float peak = 1e-6f;
                    for (int i = 0; i < kSize; ++i)
                        peak = std::max (peak, std::fabs (t[i]));
                    norm = 0.9f / peak;
                }
                for (int i = 0; i < kSize; ++i)
                    t[i] *= norm;
                t[kSize] = t[0];
            }
        }
    }
};

} // namespace kraken
