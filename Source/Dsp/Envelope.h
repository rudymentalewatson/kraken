#pragma once
#include "Common.h"

namespace kraken
{

// ADSR with a gentle attack and exponential decay / release (analog-like).
struct Adsr
{
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    Stage stage = Stage::Idle;
    float level = 0.f;

    // Set the times (seconds) and sustain level. Cheap, can be called every chunk.
    void set (float attack, float decay, float sustain, float release, float sampleRate)
    {
        sus = clampf (sustain, 0.f, 1.f);
        // Attack aims slightly above 1 so the curve is round at the start and snappy at the end.
        attCoef = 1.f - std::exp (-4.f / std::max (1.f, attack * sampleRate));
        decCoef = std::exp (-4.6f / std::max (1.f, decay * sampleRate));
        relCoef = std::exp (-4.6f / std::max (1.f, release * sampleRate));
    }

    void gateOn()
    {
        stage = Stage::Attack;   // keep current level: re-triggers do not click
    }

    void gateOff()
    {
        if (stage != Stage::Idle)
            stage = Stage::Release;
    }

    void kill()
    {
        stage = Stage::Idle;
        level = 0.f;
    }

    bool isIdle() const { return stage == Stage::Idle; }

    inline float process()
    {
        switch (stage)
        {
            case Stage::Idle:
                break;
            case Stage::Attack:
                level += (1.15f - level) * attCoef;
                if (level >= 1.f)
                {
                    level = 1.f;
                    stage = Stage::Decay;
                }
                break;
            case Stage::Decay:
                level = sus + (level - sus) * decCoef;
                if (level - sus < 1e-5f)
                {
                    level = sus;
                    stage = Stage::Sustain;
                }
                break;
            case Stage::Sustain:
                level = sus;
                break;
            case Stage::Release:
                level *= relCoef;
                if (level < 1e-5f)
                {
                    level = 0.f;
                    stage = Stage::Idle;
                }
                break;
        }
        return level;
    }

private:
    float sus = 1.f, attCoef = 0.01f, decCoef = 0.999f, relCoef = 0.999f;
};

} // namespace kraken
