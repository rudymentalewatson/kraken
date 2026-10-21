#pragma once
#include "Common.h"

namespace kraken
{

// Delay line with fractional (interpolated) reading.
class DelayLine
{
public:
    void prepare (int maxSamples)
    {
        size = 1;
        while (size < maxSamples + 4) size <<= 1;
        buf.assign ((size_t) size, 0.f);
        mask = size - 1;
        pos = 0;
    }

    void clear() { std::fill (buf.begin(), buf.end(), 0.f); }

    inline void push (float x)
    {
        pos = (pos + 1) & mask;
        buf[(size_t) pos] = x;
    }

    // delay in samples (>= 1)
    inline float read (float delay) const
    {
        const float d = clampf (delay, 1.f, (float) (size - 3));
        const int   i = (int) d;
        const float f = d - (float) i;
        const float a = buf[(size_t) ((pos - i) & mask)];
        const float b = buf[(size_t) ((pos - i - 1) & mask)];
        return a + f * (b - a);
    }

private:
    std::vector<float> buf;
    int size = 1, mask = 0, pos = 0;
};

// Three-voice ensemble chorus: mono in, wide stereo out.
class Ensemble
{
public:
    void prepare (float sr)
    {
        sampleRate = sr;
        line.prepare ((int) (0.05f * sr));
        phase = 0.f;
    }

    void process (float in, float mix, float depth, float rate, float& outL, float& outR)
    {
        line.push (in);

        phase = wrap01 (phase + rate / sampleRate);
        const float baseMs = 9.f;
        const float depthMs = 1.f + 5.f * depth;

        float v[3];
        for (int i = 0; i < 3; ++i)
        {
            const float p = phase + (float) i / 3.f;
            const float l = 0.75f * std::sin (kTwoPi * p) + 0.25f * std::sin (kTwoPi * (p * 5.3f + 0.2f * (float) i));
            const float ms = baseMs + depthMs * l;
            v[i] = line.read (ms * 0.001f * sampleRate);
        }

        const float dry = 1.f - 0.45f * mix;
        outL = in * dry + mix * (0.62f * v[0] + 0.40f * v[1]);
        outR = in * dry + mix * (0.62f * v[2] + 0.40f * v[1]);
    }

private:
    DelayLine line;
    float sampleRate = 44100.f;
    float phase = 0.f;
};

// Stereo ping-pong delay with a darker repeat.
class StereoDelay
{
public:
    void prepare (float sr)
    {
        sampleRate = sr;
        l.prepare ((int) (1.3f * sr));
        r.prepare ((int) (1.3f * sr));
        lpL = lpR = 0.f;
        smoothedTime = 0.4f * sr;
    }

    void process (float& inL, float& inR, float mix, float timeSec, float feedback)
    {
        const float target = timeSec * sampleRate;
        smoothedTime += (target - smoothedTime) * 0.0008f;

        const float dl = l.read (smoothedTime);
        const float dr = r.read (smoothedTime);

        // darker repeats
        lpL += 0.35f * (dl - lpL);
        lpR += 0.35f * (dr - lpR);

        l.push (inL + lpR * feedback);   // cross-feed = ping-pong
        r.push (inR + lpL * feedback);

        inL += mix * dl;
        inR += mix * dr;
    }

private:
    DelayLine l, r;
    float sampleRate = 44100.f;
    float lpL = 0.f, lpR = 0.f, smoothedTime = 0.f;
};

// Classic Schroeder/Moorer reverb (8 combs + 4 all-passes per side).
class Reverb
{
public:
    void prepare (float sr)
    {
        static const int combTuning[8]  = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
        static const int allpassTuning[4] = { 556, 441, 341, 225 };
        const float scale = sr / 44100.f;
        const int spread = (int) (23.f * scale);

        for (int i = 0; i < 8; ++i)
        {
            combL[i].init ((int) ((float) combTuning[i] * scale));
            combR[i].init ((int) ((float) combTuning[i] * scale) + spread);
        }
        for (int i = 0; i < 4; ++i)
        {
            apL[i].init ((int) ((float) allpassTuning[i] * scale));
            apR[i].init ((int) ((float) allpassTuning[i] * scale) + spread);
        }
    }

    void process (float& inL, float& inR, float mix, float size, float damp)
    {
        const float feedback = 0.70f + 0.28f * size;
        const float damping = 0.1f + 0.7f * damp;
        const float input = (inL + inR) * 0.015f;

        float oL = 0.f, oR = 0.f;
        for (int i = 0; i < 8; ++i)
        {
            oL += combL[i].process (input, feedback, damping);
            oR += combR[i].process (input, feedback, damping);
        }
        for (int i = 0; i < 4; ++i)
        {
            oL = apL[i].process (oL);
            oR = apR[i].process (oR);
        }

        const float wet = mix * 3.f;
        const float dry = 1.f - 0.35f * mix;
        inL = inL * dry + oL * wet;
        inR = inR * dry + oR * wet;
    }

private:
    struct Comb
    {
        std::vector<float> buf;
        int idx = 0;
        float store = 0.f;
        void init (int n) { buf.assign ((size_t) std::max (1, n), 0.f); idx = 0; store = 0.f; }
        inline float process (float in, float feedback, float damp)
        {
            const float out = buf[(size_t) idx];
            store = out * (1.f - damp) + store * damp;
            buf[(size_t) idx] = in + store * feedback;
            if (++idx >= (int) buf.size()) idx = 0;
            return out;
        }
    };

    struct Allpass
    {
        std::vector<float> buf;
        int idx = 0;
        void init (int n) { buf.assign ((size_t) std::max (1, n), 0.f); idx = 0; }
        inline float process (float in)
        {
            const float b = buf[(size_t) idx];
            const float out = b - in;
            buf[(size_t) idx] = in + b * 0.5f;
            if (++idx >= (int) buf.size()) idx = 0;
            return out;
        }
    };

    Comb combL[8], combR[8];
    Allpass apL[4], apR[4];
};

} // namespace kraken
