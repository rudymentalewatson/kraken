#include "PluginEditor.h"

using namespace kraken;

namespace
{
    constexpr int kMargin = 8, kGap = 6, kPad = 8, kTitleH = 16;
    constexpr int kCellW = 60, kCellH = 84, kLabelH = 12;

    const juce::Colour kBgTop    { 0xff0b0814 };
    const juce::Colour kBgBottom { 0xff191030 };
    const juce::Colour kPanel    { 0xff1a1530 };
    const juce::Colour kPanelEdge{ 0xff2c2450 };
    const juce::Colour kText     { 0xffd9d2ee };

    juce::Font makeFont (float size, int style = juce::Font::plain)
    {
        return juce::Font (juce::FontOptions (size, style));
    }

    int maxColumnsFor (const juce::String& section)
    {
        if (section == "Mix")        return 5;
        if (section == "FM")         return 6;
        if (section == "Wave/Super") return 3;
        if (section == "Ensemble" || section == "Delay" || section == "Reverb") return 3;
        return 4;
    }
}

//==============================================================================
KrakenLookAndFeel::KrakenLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff241d3d));
    setColour (juce::ComboBox::textColourId, kText);
    setColour (juce::ComboBox::outlineColourId, kPanelEdge);
    setColour (juce::ComboBox::arrowColourId, juce::Colour (0xffff4fa3));
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff1a1530));
    setColour (juce::PopupMenu::textColourId, kText);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xffff2e93));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::TextButton::buttonColourId, juce::Colour (0xff241d3d));
    setColour (juce::TextButton::textColourOffId, juce::Colour (0xffff4fa3));
}

void KrakenLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float startAngle, float endAngle,
                                          juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (5.f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const float angle = startAngle + sliderPos * (endAngle - startAngle);
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    // track and value arc
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.f, startAngle, endAngle, true);
    g.setColour (juce::Colour (0xff2b2445));
    g.strokePath (track, juce::PathStrokeType (3.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path value;
    value.addCentredArc (centre.x, centre.y, radius, radius, 0.f, startAngle, angle, true);
    g.setColour (accent.withAlpha (0.25f));
    g.strokePath (value, juce::PathStrokeType (6.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (accent);
    g.strokePath (value, juce::PathStrokeType (3.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // knob body
    const float kr = radius * 0.70f;
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff4d4568), centre.x, centre.y - kr,
                                             juce::Colour (0xff15111f), centre.x, centre.y + kr, false));
    g.fillEllipse (centre.x - kr, centre.y - kr, kr * 2.f, kr * 2.f);
    g.setColour (juce::Colour (0xff0b0814));
    g.drawEllipse (centre.x - kr, centre.y - kr, kr * 2.f, kr * 2.f, 1.2f);

    // pointer
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.6f, -kr + 2.f, 3.2f, kr * 0.5f, 1.5f);
    g.setColour (accent.brighter (0.4f));
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
}

juce::Label* KrakenLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = juce::LookAndFeel_V4::createSliderTextBox (slider);
    l->setFont (makeFont (10.5f));
    l->setColour (juce::Label::textColourId, kText);
    l->setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    l->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    return l;
}

//==============================================================================
juce::Colour ColourKeyboard::colourFor (int midiNoteNumber)
{
    static const juce::Colour byPitchClass[12] = {
        juce::Colour (0xffff6b8b), // C
        juce::Colour (0xffff6b8b), // C#
        juce::Colour (0xffffa94d), // D
        juce::Colour (0xffffa94d), // D#
        juce::Colour (0xffffe066), // E
        juce::Colour (0xff8be36b), // F
        juce::Colour (0xff8be36b), // F#
        juce::Colour (0xff4dd7e8), // G
        juce::Colour (0xff4dd7e8), // G#
        juce::Colour (0xff6b8cff), // A
        juce::Colour (0xff6b8cff), // A#
        juce::Colour (0xffb78bff)  // B
    };
    return byPitchClass[((midiNoteNumber % 12) + 12) % 12];
}

void ColourKeyboard::drawWhiteNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                    bool isDown, bool isOver, juce::Colour lineColour, juce::Colour)
{
    auto c = colourFor (midiNoteNumber).withMultipliedBrightness (0.88f);
    if (isDown)      c = c.brighter (0.7f);
    else if (isOver) c = c.brighter (0.2f);

    g.setColour (c);
    g.fillRect (area);
    g.setColour (lineColour);
    g.drawRect (area, 1.f);
}

void ColourKeyboard::drawBlackNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                    bool isDown, bool isOver, juce::Colour)
{
    auto c = juce::Colour (0xff15111f);
    if (isDown)      c = colourFor (midiNoteNumber).darker (0.3f);
    else if (isOver) c = c.brighter (0.2f);

    g.setColour (c);
    g.fillRect (area);
    g.setColour (colourFor (midiNoteNumber).withAlpha (0.55f));
    g.drawRect (area, 1.f);
}

//==============================================================================
KrakenEditor::KrakenEditor (KrakenProcessor& p)
    : juce::AudioProcessorEditor (&p),
      processor (p),
      keyboard (p.keyboardState, juce::MidiKeyboardComponent::verticalKeyboardFacingLeft)
{
    setLookAndFeel (&lookAndFeel);

    // Group the parameters by section, keeping the order of the registry.
    static const juce::Colour palette[] = {
        juce::Colour (0xffff2e93), juce::Colour (0xff19d3ff), juce::Colour (0xffffb000), juce::Colour (0xff9b5cff),
        juce::Colour (0xff7dff6b), juce::Colour (0xffff6b4a), juce::Colour (0xff2ee6a6), juce::Colour (0xffff7ad9),
        juce::Colour (0xff4d7cff), juce::Colour (0xffffe14d), juce::Colour (0xffff8c32)
    };

    for (int i = 0; i < NumParams; ++i)
    {
        const juce::String name (kParamDefs[i].section);
        Section* s = nullptr;
        for (auto& existing : sections)
            if (existing.name == name) { s = &existing; break; }

        if (s == nullptr)
        {
            sections.push_back ({});
            s = &sections.back();
            s->name = name;
            s->colour = palette[(sections.size() - 1) % (sizeof (palette) / sizeof (palette[0]))];
            s->maxColumns = maxColumnsFor (name);
        }
        s->params.push_back (i);
    }

    // One knob per parameter.
    knobs.resize ((size_t) NumParams);
    for (const auto& section : sections)
    {
        for (int id : section.params)
        {
            auto k = std::make_unique<Knob>();

            k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 13);
            k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                                           juce::MathConstants<float>::pi * 2.75f, true);
            k->slider.setColour (juce::Slider::rotarySliderFillColourId, section.colour);
            k->slider.setColour (juce::Slider::textBoxTextColourId, kText);
            k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
            addAndMakeVisible (k->slider);

            k->label.setText (kParamDefs[id].name, juce::dontSendNotification);
            k->label.setJustificationType (juce::Justification::centred);
            k->label.setFont (makeFont (10.5f, juce::Font::bold));
            k->label.setColour (juce::Label::textColourId, section.colour.brighter (0.3f));
            k->label.setInterceptsMouseClicks (false, false);
            addAndMakeVisible (k->label);

            k->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.apvts, kParamDefs[id].id, k->slider);

            knobs[(size_t) id] = std::move (k);
        }
    }

    // Presets
    for (int i = 0; i < processor.getNumPrograms(); ++i)
        presetBox.addItem (processor.getProgramName (i), i + 1);
    presetBox.setSelectedItemIndex (processor.getCurrentProgram(), juce::dontSendNotification);
    presetBox.onChange = [this] { processor.setCurrentProgram (presetBox.getSelectedItemIndex()); };
    addAndMakeVisible (presetBox);

    prevButton.onClick = [this] { stepPreset (-1); };
    nextButton.onClick = [this] { stepPreset (+1); };
    addAndMakeVisible (prevButton);
    addAndMakeVisible (nextButton);

    // Keyboard
    keyboard.setAvailableRange (36, 96);              // C2 .. C7
    keyboard.setScrollButtonsVisible (false);
    keyboard.setVelocity (0.8f, true);
    addAndMakeVisible (keyboard);

    setResizable (false, false);
    const int height = layoutSections();
    setSize (kMainWidth + kKeyboardWidth, height);

    startTimerHz (10);
}

KrakenEditor::~KrakenEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

int KrakenEditor::layoutSections()
{
    const int availableRight = kMainWidth - kMargin;
    int x = kMargin;
    int y = kHeaderHeight;
    int shelfHeight = 0;

    for (auto& s : sections)
    {
        const int count = (int) s.params.size();
        const int cols = juce::jmin (count, s.maxColumns);
        const int rows = (count + cols - 1) / cols;
        const int w = cols * kCellW + 2 * kPad;
        const int h = kTitleH + rows * kCellH + 2 * kPad;

        if (x + w > availableRight && x > kMargin)
        {
            x = kMargin;
            y += shelfHeight + kGap;
            shelfHeight = 0;
        }

        s.bounds = { x, y, w, h };

        for (int j = 0; j < count; ++j)
        {
            const int col = j % cols, row = j / cols;
            const int cx = x + kPad + col * kCellW;
            const int cy = y + kTitleH + kPad + row * kCellH;
            auto& k = *knobs[(size_t) s.params[(size_t) j]];
            k.label.setBounds (cx, cy, kCellW, kLabelH);
            k.slider.setBounds (cx, cy + kLabelH, kCellW, kCellH - kLabelH);
        }

        x += w + kGap;
        shelfHeight = juce::jmax (shelfHeight, h);
    }

    return y + shelfHeight + kMargin;
}

void KrakenEditor::resized()
{
    const int height = layoutSections();
    juce::ignoreUnused (height);

    // preset selector in the header
    prevButton.setBounds (kMargin, kHeaderHeight - 36, 26, 26);
    presetBox.setBounds (kMargin + 30, kHeaderHeight - 36, 210, 26);
    nextButton.setBounds (kMargin + 244, kHeaderHeight - 36, 26, 26);

    // the keyboard stands on the right edge
    auto area = juce::Rectangle<int> (kMainWidth + 4, 8, kKeyboardWidth - 12, getHeight() - 16);
    keyboard.setBounds (area);
    keyboard.setKeyWidth (juce::jmax (8.f, (float) area.getHeight() / 36.f));
}

void KrakenEditor::timerCallback()
{
    const int program = processor.getCurrentProgram();
    if (presetBox.getSelectedItemIndex() != program)
        presetBox.setSelectedItemIndex (program, juce::dontSendNotification);
}

void KrakenEditor::stepPreset (int delta)
{
    const int n = processor.getNumPrograms();
    const int next = (presetBox.getSelectedItemIndex() + delta + n) % n;
    presetBox.setSelectedItemIndex (next, juce::sendNotificationSync);
}

void KrakenEditor::paint (juce::Graphics& g)
{
    const auto full = getLocalBounds().toFloat();

    g.setGradientFill (juce::ColourGradient (kBgTop, 0.f, 0.f, kBgBottom, 0.f, full.getHeight(), false));
    g.fillAll();

    // ---- header: synthwave sun, title ----
    {
        juce::Graphics::ScopedSaveState save (g);
        g.reduceClipRegion (0, 0, kMainWidth, kHeaderHeight - 2);

        const float sunR = 62.f;
        const float sx = (float) kMainWidth - 130.f, sy = (float) kHeaderHeight - 28.f;
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xffffd23f), sx, sy - sunR,
                                                 juce::Colour (0xffff2e93), sx, sy + sunR, false));
        g.fillEllipse (sx - sunR, sy - sunR, sunR * 2.f, sunR * 2.f);

        // horizontal slits in the lower half of the sun, wider towards the bottom
        g.setColour (kBgTop);
        float yy = sy - 6.f;
        float slit = 2.f;
        while (yy < sy + sunR)
        {
            g.fillRect (sx - sunR, yy, sunR * 2.f, slit);
            yy += slit + 7.f;
            slit += 0.9f;
        }

        // fade the sun out at the bottom of the header
        g.setGradientFill (juce::ColourGradient (juce::Colours::transparentBlack, 0.f, (float) kHeaderHeight - 30.f,
                                                 kBgBottom.withAlpha (0.95f), 0.f, (float) kHeaderHeight, false));
        g.fillRect (0.f, (float) kHeaderHeight - 30.f, (float) kMainWidth, 30.f);
    }

    g.setColour (juce::Colour (0xffff2e93));
    g.fillRect (0, kHeaderHeight - 2, kMainWidth, 2);

    g.setFont (makeFont (38.f, juce::Font::bold));
    g.setColour (juce::Colour (0xffff4fa3).withAlpha (0.35f));
    g.drawText ("KRAKEN", kMargin + 2, 10 + 2, 220, 42, juce::Justification::centredLeft);
    g.setColour (juce::Colours::white);
    g.drawText ("KRAKEN", kMargin, 10, 220, 42, juce::Justification::centredLeft);

    g.setFont (makeFont (10.5f, juce::Font::bold));
    g.setColour (kText.withAlpha (0.65f));
    g.drawText ("16 OSCILLATORS  /  FM + ANALOG + WAVE", kMargin + 2, 50, 300, 14, juce::Justification::centredLeft);

    // ---- section cards ----
    for (const auto& s : sections)
    {
        const auto b = s.bounds.toFloat();
        g.setColour (kPanel);
        g.fillRoundedRectangle (b, 6.f);
        g.setColour (kPanelEdge);
        g.drawRoundedRectangle (b.reduced (0.5f), 6.f, 1.f);

        g.setColour (s.colour);
        g.fillRoundedRectangle (b.getX() + 8.f, b.getY() + 7.f, 3.f, 11.f, 1.5f);

        g.setFont (makeFont (11.f, juce::Font::bold));
        g.drawText (s.name.toUpperCase(), (int) b.getX() + 16, (int) b.getY() + 5, (int) b.getWidth() - 20, 15,
                    juce::Justification::centredLeft);
    }
}
