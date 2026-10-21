#pragma once
#include "Common.h"

namespace kraken
{

// Two-pole state-variable filter (topology-preserving transform).
// Stable under fast cutoff changes, which matters for envelopes and aftertouch.
struct Svf
{
    float s1 = 0.f, s2 = 0.f;
    float g = 0.f, k = 2.f, a1 = 0.f, a2 = 0.f, a3 = 0.f;

    void reset() { s1 = s2 = 0.f; }

    // resonance 0..1
    void set (float cutoffHz, float resonance, float sampleRate)
    {
        cutoffHz = clampf (cutoffHz, 15.f, 0.45f * sampleRate);
        g  = std::tan (kPi * cutoffHz / sampleRate);
        k  = 2.f - 1.9f * clampf (resonance, 0.f, 0.985f);
        a1 = 1.f / (1.f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    struct Out { float low, band, high; };

    inline Out process (float v0)
    {
        const float v3 = v0 - s2;
        const float v1 = a1 * s1 + a2 * v3;
        const float v2 = s2 + a2 * s1 + a3 * v3;
        s1 = 2.f * v1 - s1;
        s2 = 2.f * v2 - s2;
        return { v2, v1, v0 - k * v1 - v2 };
    }
};

} // namespace kraken
