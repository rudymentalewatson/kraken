#pragma once
#include "PluginProcessor.h"

// Look of knobs, menus and buttons: dark panel, vintage knobs, amber/green display.
class KrakenLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KrakenLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    juce::Label* createSliderTextBox (juce::Slider&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
};

// Keyboard along the bottom: ivory keys with a coloured cap, like the old home keyboards.
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
        int row = 0;
        std::vector<int> params;
        juce::Rectangle<int> bounds;
    };

    void layoutPanel();
    void timerCallback() override;
    void stepPreset (int delta);
    void fillPresetMenu();

    KrakenProcessor& processor;
    KrakenLookAndFeel lookAndFeel;

    std::vector<Section> sections;
    std::vector<std::unique_ptr<Knob>> knobs;     // indexed by parameter id

    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" };
    ColourKeyboard keyboard;
    juce::Rectangle<int> displayCard, lcdBox, keyboardArea;

    // geometry (pixels)
    static constexpr int kWidth = 1120, kHeight = 566;
    static constexpr int kCheek = 44;          // wooden side panels
    static constexpr int kHeaderH = 56;
    static constexpr int kInnerW = 1020;
    static constexpr int kCellW = 54, kCellH = 76, kLabelH = 12;
    static constexpr int kBarH = 18, kPadX = 6, kPadTop = 4, kGap = 6;
    static constexpr int kCardH = kBarH + kPadTop * 2 + 2 * kCellH;
    static constexpr int kKeyboardH = 114;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KrakenEditor)
};
