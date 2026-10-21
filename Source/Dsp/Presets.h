#pragma once
#include <cstdlib>
#include <string>
#include <vector>
#include "Params.h"

namespace kraken
{

struct Preset
{
    std::string name;
    std::string category;
    Params values;
};

namespace detail
{
    struct Raw
    {
        const char* name;
        const char* category;
        // "key=value,key=value,..."; may start with "@Other Preset;" to inherit from an earlier one.
        const char* spec;
    };

    inline const std::vector<Raw>& rawBank()
    {
        static const std::vector<Raw> bank = {
            // ------------------------------------------------------------ PADS
            { "Neon Rain", "Pad", "lvlAnalog=1,lvlFM=0.10,lvlWave=0,lvlSuper=0.35,anPulse=0.25,anDetune=14,anPWM=0.5,lpfCut=1100,lpfRes=0.30,fltEnv=0.55,fltKey=0.6,fltA=1.1,fltD=2.5,fltS=0.45,fltR=2.2,ampA=0.9,ampD=2,ampS=0.85,ampR=2.4,aftFilter=0.7,lfoPitch=0.08,drift=0.5,chMix=0.6,chDepth=0.55,dlMix=0.2,rvMix=0.45,rvSize=0.9" },
            { "Replicant Dawn", "Pad", "@Neon Rain;lpfCut=700,fltEnv=0.8,fltA=3,ampA=2.2,lvlSuper=0.5,anDetune=18,rvMix=0.55,dlMix=0.3,dlFb=0.5" },
            { "Tears In Rain", "Pad", "@Neon Rain;lvlFM=0.3,fmAlgo=2,fmRatio1=3,fmRatio2=9,fmRatio3=5,fmRatio4=11,fmLvl2=0.3,fmEnv=0.8,fmDecay=3,lpfCut=2400,ampA=1.6,ampR=4,rvMix=0.6,rvSize=1,dlMix=0.25" },
            { "Glass Cathedral", "Pad", "lvlAnalog=0.2,lvlFM=0.7,lvlWave=0.6,lvlSuper=0,wtMorph=0.7,fmAlgo=4,fmRatio1=3,fmRatio2=5,fmRatio3=7,fmRatio4=9,fmLvl1=0.8,fmLvl2=0.4,fmLvl3=0.3,fmLvl4=0.25,fmFb=0,lpfCut=6000,fltEnv=0.1,ampA=1.5,ampR=4,chMix=0.6,rvMix=0.65,rvSize=1" },
            { "Supernova Wall", "Pad", "lvlAnalog=0.4,lvlFM=0,lvlWave=0.3,lvlSuper=1,ssDetune=0.9,wtMorph=0.85,lpfCut=5000,lpfRes=0.35,fltEnv=0.6,fltA=2.5,fltD=3,ampA=1.8,ampR=3,chMix=0.6,dlMix=0.3,dlFb=0.5,rvMix=0.55,rvSize=1" },
            { "Warm Blanket", "Pad", "lvlAnalog=1,lvlFM=0,lvlWave=0.2,lvlSuper=0.2,anPulse=0.5,anPWM=0.6,anDetune=9,lpfCut=900,lpfRes=0.15,fltEnv=0.25,ampA=1.6,ampR=3,chMix=0.7,rvMix=0.4,drift=0.6" },
            { "Slow Tide", "Pad", "@Warm Blanket;lfoRate=0.15,lfoFilter=0.45,lpfCut=1400,lpfRes=0.35,ampA=2.5,ampR=4,dlMix=0.3,dlFb=0.55" },
            { "Velvet Night", "Pad", "lvlAnalog=0.6,lvlFM=0.2,lvlWave=0.5,lvlSuper=0.3,wtMorph=0.2,lpfCut=1800,fltEnv=0.35,fltA=1.8,ampA=1.2,ampR=3,chMix=0.65,rvMix=0.55,rvSize=0.95" },
            { "Aurora", "Pad", "lvlAnalog=0.3,lvlFM=0.2,lvlWave=0.8,lvlSuper=0.5,wtMorph=0.9,wtDetune=18,ssDetune=0.7,lpfCut=4500,fltEnv=0.4,fltA=2,ampA=2,ampR=4,chMix=0.7,dlMix=0.3,rvMix=0.6" },
            { "Dust Storm", "Pad", "lvlAnalog=0.7,lvlFM=0,lvlWave=0.5,lvlSuper=0.6,wtMorph=0.6,anPulse=0.6,anPWM=0.9,lfoRate=0.3,lfoFilter=0.5,lpfCut=1000,lpfRes=0.5,drift=0.9,ampA=2,ampR=3.5,chMix=0.5,rvMix=0.5" },
            { "Cold Orbit", "Pad", "lvlAnalog=0.2,lvlFM=0.6,lvlWave=0.5,lvlSuper=0,wtMorph=0.8,fmAlgo=1,fmRatio1=3,fmRatio2=4,fmRatio3=3,fmRatio4=8,fmLvl2=0.3,fmEnv=0.5,fmDecay=4,lpfCut=3500,ampA=1.8,ampR=4,chMix=0.4,dlMix=0.35,dlFb=0.6,rvMix=0.55" },
            { "Memory Lane", "Pad", "@Neon Rain;lvlSuper=0.2,lvlWave=0.3,wtMorph=0.1,lpfCut=1600,ampA=0.5,ampR=2,chMix=0.7,rvMix=0.4,dlMix=0.15" },

            // ----------------------------------------------------------- BRASS
            { "Off-World Brass", "Brass", "lvlAnalog=1,lvlFM=0,lvlWave=0,lvlSuper=0.5,anPulse=0.2,anDetune=10,hpfCut=120,lpfCut=700,lpfRes=0.2,fltEnv=0.8,fltA=0.35,fltD=1.2,fltS=0.55,fltR=0.9,ampA=0.25,ampD=1,ampS=0.9,ampR=0.9,aftFilter=0.9,lfoPitch=0.1,chMix=0.45,dlMix=0.1,rvMix=0.3" },
            { "Imperial Brass", "Brass", "@Off-World Brass;lpfCut=900,fltA=0.5,ampA=0.35,lvlSuper=0.7,rvMix=0.4" },
            { "Soft Horns", "Brass", "@Off-World Brass;lvlSuper=0.3,lpfCut=500,fltEnv=0.6,fltA=0.2,ampA=0.18,ampR=0.5,chMix=0.3,rvMix=0.2" },
            { "Analog Trumpet", "Brass", "lvlAnalog=1,lvlSuper=0.2,lvlFM=0,lvlWave=0,anPulse=0.4,anDetune=6,hpfCut=180,lpfCut=600,lpfRes=0.3,fltEnv=0.9,fltA=0.08,fltD=0.5,fltS=0.5,fltR=0.3,ampA=0.05,ampD=0.5,ampS=0.85,ampR=0.3,lfoPitch=0.12,lfoRate=5,chMix=0.25,rvMix=0.2,aftFilter=0.8" },
            { "Brass Stab", "Brass", "@Analog Trumpet;ampA=0.01,ampD=0.5,ampS=0,ampR=0.3,fltA=0.01,fltD=0.4,fltS=0.1,lvlSuper=0.5,rvMix=0.3,dlMix=0.2" },
            { "Slow Brass", "Brass", "@Off-World Brass;fltA=1.8,ampA=1.2,ampR=1.6,rvMix=0.45,chMix=0.5" },

            // --------------------------------------------------------- STRINGS
            { "Lunar Strings", "Strings", "lvlAnalog=0.5,lvlFM=0,lvlWave=0.2,lvlSuper=0.8,wtMorph=0.2,anPulse=0.1,ssDetune=0.5,lpfCut=3200,fltEnv=0.2,ampA=1.4,ampD=2,ampS=0.9,ampR=2.8,chMix=0.8,chDepth=0.7,rvMix=0.5" },
            { "Ensemble 84", "Strings", "lvlAnalog=0.4,lvlFM=0,lvlWave=0.1,lvlSuper=1,ssDetune=0.45,anPulse=0.2,lpfCut=3800,lpfRes=0.1,fltEnv=0.1,ampA=0.6,ampD=1,ampS=0.9,ampR=1.2,chMix=0.9,chDepth=0.75,rvMix=0.35" },
            { "Silk Strings", "Strings", "@Ensemble 84;ssDetune=0.3,lpfCut=2400,ampA=1.1,ampR=2,chMix=0.75,rvMix=0.5" },
            { "Cello Pad", "Strings", "lvlAnalog=0.8,lvlSuper=0.4,lvlWave=0.2,anPulse=0.3,anDetune=8,wtMorph=0.15,lpfCut=900,lpfRes=0.2,fltEnv=0.3,fltA=0.5,ampA=0.5,ampS=0.9,ampR=1.5,lfoPitch=0.15,lfoRate=5.5,chMix=0.4,rvMix=0.4" },
            { "Dark Strings", "Strings", "@Ensemble 84;lpfCut=1100,lpfRes=0.3,fltEnv=0.3,ampA=0.9,ampR=2.5,rvMix=0.6,rvDamp=0.7" },

            // ----------------------------------------------------------- CHOIR
            { "Warm Choir", "Choir", "lvlAnalog=0.3,lvlFM=0,lvlWave=1,lvlSuper=0,wtMorph=0.33,wtDetune=12,lpfCut=2800,fltEnv=0.2,ampA=1,ampS=0.9,ampR=2.5,chMix=0.7,rvMix=0.5" },
            { "Ghost Choir", "Choir", "@Warm Choir;wtMorph=0.5,lvlWave=1,lvlAnalog=0.1,lfoRate=0.4,lfoFilter=0.25,ampA=1.6,ampR=3.5,rvMix=0.65,dlMix=0.2" },
            { "Cathedral Voices", "Choir", "@Warm Choir;lvlSuper=0.2,wtDetune=18,ampA=1.2,ampR=3.5,rvMix=0.75,rvSize=1,chMix=0.8" },
            { "Vox Humana", "Choir", "lvlAnalog=0,lvlFM=0,lvlWave=1,lvlSuper=0,wtMorph=0.38,wtDetune=7,lpfCut=3000,fltEnv=0.1,ampA=0.35,ampR=1.4,lfoPitch=0.12,lfoRate=5.2,chMix=0.5,rvMix=0.45" },

            // ------------------------------------------------------------ KEYS
            { "Electric Keys", "Keys", "lvlAnalog=0.1,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=2,fmRatio1=3,fmRatio2=11,fmRatio3=3,fmRatio4=3,fmLvl1=1,fmLvl2=0.25,fmLvl3=0.5,fmLvl4=0.3,fmFb=0,fmEnv=0.8,fmDecay=0.6,lpfCut=6000,fltEnv=0,fltKey=0.7,ampA=0.003,ampD=2.2,ampS=0,ampR=0.6,chMix=0.6,rvMix=0.25,dlMix=0.05" },
            { "Tine Piano", "Keys", "@Electric Keys;fmRatio2=9,fmLvl2=0.18,fmDecay=1.2,ampD=3,rvMix=0.3,chMix=0.4" },
            { "Dream Keys", "Keys", "@Electric Keys;lvlAnalog=0.25,lvlWave=0.2,wtMorph=0.1,fmLvl2=0.15,fmDecay=1.8,ampD=3.5,ampR=1.4,chMix=0.8,rvMix=0.5,dlMix=0.2" },
            { "Clav FM", "Keys", "lvlAnalog=0.1,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=0,fmRatio1=3,fmRatio2=3,fmRatio3=5,fmRatio4=3,fmLvl1=1,fmLvl2=0.6,fmLvl3=0.3,fmLvl4=0.2,fmFb=0.35,fmEnv=0.9,fmDecay=0.25,hpfCut=200,lpfCut=7000,fltEnv=0,ampA=0.003,ampD=0.5,ampS=0,ampR=0.15,chMix=0.15,rvMix=0.1,dlMix=0" },
            { "Drawbar Organ", "Keys", "lvlAnalog=0,lvlFM=0.6,lvlWave=1,lvlSuper=0,wtMorph=0,wtDetune=0,fmAlgo=4,fmRatio1=3,fmRatio2=5,fmRatio3=6,fmRatio4=7,fmLvl1=0.8,fmLvl2=0.6,fmLvl3=0.4,fmLvl4=0.3,fmFb=0,fmEnv=0,lpfCut=9000,fltEnv=0,fltKey=0.8,ampA=0.01,ampD=0.1,ampS=1,ampR=0.08,lfoRate=6,lfoPitch=0.05,chMix=0.8,chRate=0.8,rvMix=0.2" },
            { "Church Organ", "Keys", "@Drawbar Organ;ampA=0.05,ampR=0.25,rvMix=0.7,rvSize=1,chMix=0.4" },
            { "Glass Keys", "Keys", "@Electric Keys;fmRatio1=5,fmRatio2=9,fmRatio3=7,fmRatio4=11,lvlWave=0.15,wtMorph=0.8,fmDecay=1.5,ampD=2.5,rvMix=0.4,dlMix=0.2" },
            { "Marimba FM", "Keys", "lvlAnalog=0,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=2,fmRatio1=3,fmRatio2=10,fmRatio3=3,fmRatio4=3,fmLvl1=1,fmLvl2=0.5,fmLvl3=0.2,fmLvl4=0,fmFb=0,fmEnv=1,fmDecay=0.15,lpfCut=8000,fltEnv=0,ampA=0.002,ampD=0.9,ampS=0,ampR=0.2,chMix=0.2,rvMix=0.25" },

            // ----------------------------------------------------------- BELLS
            { "Glass Bells", "Bells", "lvlAnalog=0,lvlFM=1,lvlWave=0.15,lvlSuper=0,wtMorph=0.7,fmAlgo=2,fmRatio1=3,fmRatio2=9,fmRatio3=5,fmRatio4=11,fmLvl1=1,fmLvl2=0.55,fmLvl3=0.7,fmLvl4=0.4,fmFb=0,fmEnv=0.9,fmDecay=2.5,lpfCut=9000,fltEnv=0,fltKey=0.8,ampA=0.003,ampD=3.5,ampS=0,ampR=2.5,chMix=0.35,dlMix=0.3,rvMix=0.4" },
            { "Crystal Chimes", "Bells", "@Glass Bells;fmRatio1=5,fmRatio2=11,fmRatio3=9,fmRatio4=11,fmLvl2=0.4,ampD=4,ampR=3,dlMix=0.35,rvMix=0.5" },
            { "Temple Bells", "Bells", "@Glass Bells;fmRatio1=2,fmRatio2=8,fmRatio3=4,fmRatio4=10,fmLvl2=0.7,fmLvl3=0.5,ampD=5,ampR=4,rvMix=0.55,rvSize=1,lvlWave=0" },
            { "Tubular", "Bells", "lvlAnalog=0,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=2,fmRatio1=3,fmRatio2=8,fmRatio3=5,fmRatio4=9,fmLvl1=1,fmLvl2=0.7,fmLvl3=0.5,fmLvl4=0.2,fmEnv=0.7,fmDecay=3,lpfCut=10000,ampA=0.002,ampD=5,ampS=0,ampR=3,chMix=0.2,rvMix=0.45" },
            { "Music Box", "Bells", "lvlAnalog=0,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=2,fmRatio1=5,fmRatio2=11,fmRatio3=7,fmRatio4=11,fmLvl1=1,fmLvl2=0.25,fmLvl3=0.3,fmLvl4=0.1,fmEnv=1,fmDecay=0.4,lpfCut=12000,ampA=0.002,ampD=1.4,ampS=0,ampR=0.8,chMix=0.1,rvMix=0.3,dlMix=0.2" },

            // ------------------------------------------------------------ BASS
            { "Sub Bass", "Bass", "lvlAnalog=0.7,lvlFM=0.3,lvlWave=0,lvlSuper=0,anPulse=0,anDetune=4,fmAlgo=0,fmRatio1=3,fmRatio2=3,fmLvl1=1,fmLvl2=0.1,fmEnv=0,lpfCut=300,lpfRes=0.1,fltEnv=0,fltKey=0.2,ampA=0.005,ampD=0.5,ampS=0.9,ampR=0.15,chMix=0,rvMix=0.05,dlMix=0" },
            { "FM Bass", "Bass", "lvlAnalog=0.3,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=0,fmRatio1=3,fmRatio2=3,fmRatio3=5,fmRatio4=3,fmLvl1=1,fmLvl2=0.7,fmLvl3=0.3,fmLvl4=0.2,fmFb=0.4,fmEnv=0.8,fmDecay=0.3,lpfCut=1400,fltEnv=0,ampA=0.003,ampD=0.5,ampS=0.7,ampR=0.15,chMix=0,rvMix=0.05,dlMix=0" },
            { "Analog Bass", "Bass", "lvlAnalog=1,lvlFM=0,lvlWave=0,lvlSuper=0,anPulse=0.3,anDetune=7,lpfCut=350,lpfRes=0.5,fltEnv=0.6,fltA=0.003,fltD=0.35,fltS=0.15,fltR=0.2,fltKey=0.3,ampA=0.003,ampD=0.4,ampS=0.8,ampR=0.12,chMix=0.05,rvMix=0.05,dlMix=0" },
            { "Wobble Bass", "Bass", "@Analog Bass;lfoRate=3,lfoFilter=0.8,lpfCut=450,lpfRes=0.6" },
            { "Deep Pulse", "Bass", "lvlAnalog=1,lvlFM=0,lvlWave=0,lvlSuper=0,anPulse=1,anPW=0.35,anPWM=0.6,anDetune=5,lfoRate=0.7,lpfCut=500,lpfRes=0.25,fltEnv=0.4,fltD=0.5,fltS=0.3,ampA=0.005,ampS=0.9,ampR=0.2,chMix=0.25,rvMix=0.1,dlMix=0" },
            { "Reso Bass", "Bass", "@Analog Bass;lpfCut=250,lpfRes=0.8,fltEnv=0.9,fltD=0.2,fltS=0" },

            // ------------------------------------------------------------ LEAD
            { "Singing Lead", "Lead", "lvlAnalog=1,lvlSuper=0.3,lvlFM=0,lvlWave=0.2,wtMorph=0.38,anPulse=0.3,anDetune=8,hpfCut=100,lpfCut=2600,lpfRes=0.3,fltEnv=0.4,fltA=0.01,fltD=0.6,fltS=0.5,fltR=0.5,ampA=0.02,ampD=0.5,ampS=0.9,ampR=0.5,lfoRate=5.2,lfoPitch=0.2,aftFilter=0.6,chMix=0.35,dlMix=0.25,dlFb=0.4,rvMix=0.25" },
            { "Saw Lead", "Lead", "lvlAnalog=0.4,lvlSuper=1,lvlFM=0,lvlWave=0,ssDetune=0.35,lpfCut=4500,lpfRes=0.2,fltEnv=0.3,fltA=0.01,fltD=0.7,fltS=0.6,ampA=0.01,ampS=0.9,ampR=0.4,lfoRate=5.5,lfoPitch=0.1,chMix=0.4,dlMix=0.3,rvMix=0.25" },
            { "Pulse Lead", "Lead", "lvlAnalog=1,lvlSuper=0,lvlFM=0,lvlWave=0,anPulse=1,anPW=0.3,anPWM=0.5,lfoRate=0.9,anDetune=10,lpfCut=3500,lpfRes=0.3,fltEnv=0.35,fltD=0.4,fltS=0.4,ampA=0.01,ampS=0.85,ampR=0.3,chMix=0.4,dlMix=0.25,rvMix=0.2" },
            { "FM Lead", "Lead", "lvlAnalog=0.3,lvlFM=1,lvlWave=0,lvlSuper=0.1,fmAlgo=1,fmRatio1=3,fmRatio2=3,fmRatio3=5,fmRatio4=3,fmLvl1=1,fmLvl2=0.5,fmLvl3=0.3,fmLvl4=0.3,fmFb=0.5,fmEnv=0.5,fmDecay=1.5,lpfCut=6000,ampA=0.01,ampS=0.9,ampR=0.3,lfoPitch=0.12,lfoRate=5.3,chMix=0.3,dlMix=0.3,rvMix=0.2" },
            { "Soft Lead", "Lead", "@Singing Lead;lvlSuper=0,lpfCut=1600,lpfRes=0.1,anPulse=0.1,ampA=0.08,lfoPitch=0.12" },

            // ----------------------------------------------------------- PLUCK
            { "Pluck Harp", "Pluck", "lvlAnalog=0.3,lvlFM=0.8,lvlWave=0,lvlSuper=0,fmAlgo=0,fmRatio1=3,fmRatio2=5,fmRatio3=3,fmRatio4=3,fmLvl1=1,fmLvl2=0.3,fmEnv=1,fmDecay=0.4,lpfCut=5000,fltEnv=0.3,fltD=0.4,fltS=0,ampA=0.002,ampD=1.6,ampS=0,ampR=0.5,chMix=0.4,rvMix=0.4,dlMix=0.2" },
            { "Pizzicato", "Pluck", "lvlAnalog=1,lvlFM=0,lvlWave=0,lvlSuper=0,anPulse=0,anDetune=3,lpfCut=900,lpfRes=0.2,fltEnv=0.7,fltA=0.002,fltD=0.15,fltS=0,ampA=0.002,ampD=0.3,ampS=0,ampR=0.12,rvMix=0.25,chMix=0.2" },
            { "Kalimba", "Pluck", "lvlAnalog=0,lvlFM=1,lvlWave=0,lvlSuper=0,fmAlgo=2,fmRatio1=3,fmRatio2=10,fmRatio3=3,fmRatio4=3,fmLvl1=1,fmLvl2=0.3,fmLvl3=0.1,fmEnv=1,fmDecay=0.2,lpfCut=7000,ampA=0.002,ampD=1.1,ampS=0,ampR=0.4,rvMix=0.3,chMix=0.1" },
            { "Analog Pluck", "Pluck", "lvlAnalog=1,lvlSuper=0.3,anPulse=0.4,lpfCut=700,lpfRes=0.35,fltEnv=0.8,fltA=0.002,fltD=0.3,fltS=0,ampA=0.002,ampD=0.5,ampS=0,ampR=0.3,chMix=0.4,dlMix=0.3,rvMix=0.25" },
            { "Glass Pluck", "Pluck", "lvlAnalog=0,lvlFM=0.6,lvlWave=0.8,lvlSuper=0,wtMorph=0.8,fmAlgo=2,fmRatio1=5,fmRatio2=9,fmRatio3=7,fmRatio4=11,fmLvl2=0.3,fmEnv=1,fmDecay=0.5,lpfCut=9000,ampA=0.002,ampD=1.2,ampS=0,ampR=0.5,rvMix=0.4,dlMix=0.3,chMix=0.2" },

            // ---------------------------------------------------------- DRONE / FX
            { "Dark Drone", "FX", "lvlAnalog=1,lvlFM=0,lvlWave=0.4,lvlSuper=0,wtMorph=0.45,anPulse=0.6,anPWM=0.8,anDetune=18,lpfCut=450,lpfRes=0.45,fltEnv=0.3,lfoRate=0.12,lfoFilter=0.4,drift=0.8,ampA=3.5,ampD=3,ampS=1,ampR=5,dlMix=0.35,dlFb=0.6,rvMix=0.6" },
            { "Cave Drone", "FX", "@Dark Drone;lpfCut=300,lfoRate=0.08,lfoFilter=0.5,rvMix=0.75,rvSize=1,rvDamp=0.7" },
            { "Space Wind", "FX", "lvlAnalog=0.2,lvlFM=0,lvlWave=0.9,lvlSuper=0.7,wtMorph=1,ssDetune=1,lfoRate=0.2,lfoFilter=0.7,lpfCut=1500,lpfRes=0.6,fltEnv=0.2,drift=1,ampA=3.5,ampD=3,ampS=1,ampR=5,chMix=0.8,dlMix=0.45,dlFb=0.65,rvMix=0.7" },
            { "Alien Choir", "FX", "lvlAnalog=0,lvlFM=0.5,lvlWave=1,lvlSuper=0,wtMorph=0.55,wtDetune=25,fmAlgo=3,fmRatio1=3,fmRatio2=1,fmRatio3=2,fmRatio4=9,fmLvl2=0.5,fmEnv=0,fmFb=0.3,lfoRate=0.35,lfoPitch=0.3,lfoFilter=0.4,lpfCut=2400,ampA=1.5,ampR=3.5,chMix=0.6,rvMix=0.6,dlMix=0.3" },
            { "Tidal Breath", "FX", "lvlAnalog=0.5,lvlSuper=0.7,lvlWave=0.4,lvlFM=0,ssDetune=0.8,lfoRate=0.12,lfoFilter=0.8,lpfCut=900,lpfRes=0.55,drift=0.7,ampA=3,ampR=4,chMix=0.7,rvMix=0.65,dlMix=0.25" },
            { "Lunar Surface", "FX", "lvlAnalog=0.2,lvlFM=0.4,lvlWave=0.3,lvlSuper=0,fmAlgo=1,fmRatio1=3,fmRatio2=1,fmRatio3=4,fmRatio4=5,fmLvl2=0.3,fmLvl3=0.25,fmFb=0.1,fmEnv=0.2,fmDecay=6,lpfCut=2200,lfoRate=0.1,lfoFilter=0.4,ampA=4,ampD=4,ampS=0.8,ampR=6,dlMix=0.35,dlFb=0.55,rvMix=0.5,rvSize=0.95,drift=0.8" },

            // ------------------------------------------------------------ INIT
            { "Init Saw", "Init", "lvlAnalog=1,lvlFM=0,lvlWave=0,lvlSuper=0,anPulse=0,anDetune=6,lpfCut=8000,lpfRes=0.1,fltEnv=0,ampA=0.01,ampD=0.5,ampS=1,ampR=0.3,chMix=0,dlMix=0,rvMix=0" },
        };
        return bank;
    }

    struct Bank
    {
        std::vector<Preset> list;
        int errors = 0;     // unknown names, unknown base presets, or out-of-range values
    };

    inline Bank buildBank()
    {
        Bank b;
        for (const auto& raw : rawBank())
        {
            Preset p;
            p.name = raw.name;
            p.category = raw.category;
            p.values = defaultParams();

            const char* s = raw.spec;
            if (*s == '@')
            {
                const char* end = std::strchr (s, ';');
                if (end == nullptr) { ++b.errors; end = s; }
                const std::string baseName (s + 1, (size_t) (end - s - 1));
                bool found = false;
                for (const auto& other : b.list)
                    if (other.name == baseName) { p.values = other.values; found = true; break; }
                if (! found) ++b.errors;
                s = end + (*end == ';' ? 1 : 0);
            }

            std::string item;
            auto flush = [&]()
            {
                if (item.empty()) return;
                const size_t eq = item.find ('=');
                const int id = eq == std::string::npos ? -1 : findParam (item.substr (0, eq).c_str());
                if (id < 0) { ++b.errors; item.clear(); return; }
                const float v = std::strtof (item.c_str() + eq + 1, nullptr);
                const ParamDef& d = kParamDefs[id];
                if (v < d.minV - 1e-4f || v > d.maxV + 1e-4f) ++b.errors;
                p.values[(size_t) id] = v < d.minV ? d.minV : (v > d.maxV ? d.maxV : v);
                item.clear();
            };
            for (; *s != '\0'; ++s)
            {
                if (*s == ',') flush();
                else item.push_back (*s);
            }
            flush();

            b.list.push_back (std::move (p));
        }
        return b;
    }

    inline const Bank& bank()
    {
        static const Bank instance = buildBank();
        return instance;
    }
} // namespace detail

inline const std::vector<Preset>& factoryPresets() { return detail::bank().list; }

// 0 means every preset in the bank is valid.
inline int presetErrors() { return detail::bank().errors; }

inline Params presetParams (int index)
{
    const auto& list = factoryPresets();
    if (index < 0 || index >= (int) list.size())
        return defaultParams();
    return list[(size_t) index].values;
}

} // namespace kraken
