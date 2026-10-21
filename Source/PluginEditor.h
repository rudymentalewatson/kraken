#pragma once
#include "PluginProcessor.h"

// Look of the knobs and menus: dark graphite, neon accents.
class KrakenLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KrakenLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    juce::Label* createSliderTextBox (juce::Slider&) override;
};

// On-screen keyboard with coloured keys, standing upright on the right edge.
class ColourKeyboard : public juce::MidiKeyboardComponent
{
public:
    ColourKeyboard (juce::MidiKeyboardState& state, Orientation orientation)
        : juce::MidiKeyboardComponent (state, orientation) {}

protected:
    void drawWhiteNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                        bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour) override;
    void drawBlackNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                        bool isDown, bool isOver, juce::Colour noteFillColour) override;

private:
    static juce::Colour colourFor (int midiNoteNumber);
};

class KrakenEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    explicit KrakenEditor (KrakenProcessor&);
    ~KrakenEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    struct Section
    {
        juce::String name;
        juce::Colour colour;
        int maxColumns = 4;
        std::vector<int> params;
        juce::Rectangle<int> bounds;
    };

    int layoutSections();       // places cards and knobs, returns the total height
    void timerCallback() override;
    void stepPreset (int delta);

    KrakenProcessor& processor;
    KrakenLookAndFeel lookAndFeel;

    std::vector<Section> sections;
    std::vector<std::unique_ptr<Knob>> knobs;     // indexed by parameter id

    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" };
    ColourKeyboard keyboard;

    static constexpr int kMainWidth = 620;
    static constexpr int kKeyboardWidth = 92;
    static constexpr int kHeaderHeight = 104;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KrakenEditor)
};
