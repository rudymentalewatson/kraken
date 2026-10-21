#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace kraken;

juce::AudioProcessorValueTreeState::ParameterLayout KrakenProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const Params initial = presetParams (0);     // the plugin starts on the first preset

    for (int i = 0; i < NumParams; ++i)
    {
        const ParamDef& d = kParamDefs[i];

        juce::NormalisableRange<float> range (d.minV, d.maxV, d.step);
        if (d.centre > 0.f)
            range.setSkewForCentre (d.centre);

        auto attrs = juce::AudioParameterFloatAttributes().withLabel (d.unit);

        const juce::String id (d.id);
        if (id.startsWith ("fmRatio"))
        {
            attrs = attrs.withStringFromValueFunction ([] (float v, int)
            {
                const int idx = juce::jlimit (0, 11, (int) std::lround (v));
                return juce::String (kFmRatios[idx], 2) + "x";
            });
        }
        else if (id == "fmAlgo")
        {
            attrs = attrs.withStringFromValueFunction ([] (float v, int)
            {
                return juce::String (juce::jlimit (0, 4, (int) std::lround (v)) + 1);
            });
        }

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { d.id, 1 }, d.name, range, initial[(size_t) i], attrs));
    }
    return layout;
}

KrakenProcessor::KrakenProcessor()
    : juce::AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "KRAKEN", createLayout())
{
    for (int i = 0; i < NumParams; ++i)
        rawParams[i] = apvts.getRawParameterValue (kParamDefs[i].id);
}

void KrakenProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    engine.prepare (sampleRate, samplesPerBlock);
    spareChannel.assign ((size_t) std::max (samplesPerBlock, 512), 0.f);
    keyboardState.reset();
}

bool KrakenProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void KrakenProcessor::handleMidi (const juce::MidiMessage& m)
{
    if (m.isNoteOn())
        engine.noteOn (m.getNoteNumber(), m.getFloatVelocity());
    else if (m.isNoteOff())
        engine.noteOff (m.getNoteNumber());
    else if (m.isAllNotesOff() || m.isAllSoundOff())
        engine.allNotesOff();
    else if (m.isPitchWheel())
        engine.setPitchBend ((float) (m.getPitchWheelValue() - 8192) / 8192.f);
    else if (m.isController())
    {
        if (m.getControllerNumber() == 1)
            engine.setModWheel ((float) m.getControllerValue() / 127.f);
        else if (m.getControllerNumber() == 64)
            engine.setSustainPedal (m.getControllerValue() >= 64);
    }
    else if (m.isChannelPressure())
        engine.setChannelPressure ((float) m.getChannelPressureValue() / 127.f);
    else if (m.isAftertouch())
        engine.setPolyPressure (m.getNoteNumber(), (float) m.getAfterTouchValue() / 127.f);
}

void KrakenProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    buffer.clear();

    // notes played on the on-screen keyboard join the incoming MIDI
    keyboardState.processNextMidiBuffer (midi, 0, numSamples, true);

    Params p;
    for (int i = 0; i < NumParams; ++i)
        p[(size_t) i] = rawParams[i]->load();
    engine.setParams (p);

    float* left = buffer.getWritePointer (0);
    float* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;

    if (right == nullptr)
    {
        if ((int) spareChannel.size() < numSamples)
            spareChannel.resize ((size_t) numSamples);
        right = spareChannel.data();
    }

    int pos = 0;
    for (const auto metadata : midi)
    {
        const int t = juce::jlimit (pos, numSamples, metadata.samplePosition);
        if (t > pos)
            engine.render (left + pos, right + pos, t - pos);
        pos = t;
        handleMidi (metadata.getMessage());
    }
    if (numSamples > pos)
        engine.render (left + pos, right + pos, numSamples - pos);
}

juce::AudioProcessorEditor* KrakenProcessor::createEditor()
{
    return new KrakenEditor (*this);
}

int KrakenProcessor::getNumPrograms()
{
    return (int) factoryPresets().size();
}

void KrakenProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= getNumPrograms())
        return;

    currentProgram = index;
    const Params p = presetParams (index);

    for (int i = 0; i < NumParams; ++i)
        if (auto* param = apvts.getParameter (kParamDefs[i].id))
            param->setValueNotifyingHost (param->convertTo0to1 (p[(size_t) i]));
}

const juce::String KrakenProcessor::getProgramName (int index)
{
    const auto& list = factoryPresets();
    if (index < 0 || index >= (int) list.size())
        return {};
    return list[(size_t) index].name;
}

void KrakenProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("program", currentProgram, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void KrakenProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            currentProgram = juce::jlimit (0, getNumPrograms() - 1, (int) apvts.state.getProperty ("program", 0));
        }
}

// This creates new instances of the plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KrakenProcessor();
}
