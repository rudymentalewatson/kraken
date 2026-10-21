#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace kraken
{

constexpr float kPi    = 3.14159265358979f;
constexpr float kTwoPi = 6.28318530717959f;

inline float clampf (float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }

// Smooth saturation, close to tanh but cheap. Input is clamped so it is always safe.
inline float softClip (float x)
{
    x = clampf (x, -3.f, 3.f);
    return x * (27.f + x * x) / (27.f + 9.f * x * x);
}

// Tiny random number generator (deterministic, no allocation).
struct Rng
{
    uint32_t s = 22222u;
    void seed (uint32_t v) { s = v ? v : 1u; }
    uint32_t next()
    {
        s ^= s << 13;
        s ^= s >> 17;
        s ^= s << 5;
        return s;
    }
    float uni()  { return (float) (next() & 0xFFFFFF) / 16777216.f; }        // 0..1
    float bi()   { return uni() * 2.f - 1.f; }                              // -1..1
};

// Band-limited step correction used by the saw and pulse oscillators.
inline float polyBlep (float t, float dt)
{
    if (t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.f;
    }
    if (t > 1.f - dt)
    {
        t = (t - 1.f) / dt;
        return t * t + t + t + 1.f;
    }
    return 0.f;
}

inline float wrap01 (float p)
{
    p -= std::floor (p);
    return p;
}

// Cents -> frequency ratio
inline float centsToRatio (float cents) { return std::exp2 (cents * (1.f / 1200.f)); }

} // namespace kraken
