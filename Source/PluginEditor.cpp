#include "PluginEditor.h"

using namespace kraken;

namespace
{
    const juce::Colour kPanelTop    { 0xff2a2830 };
    const juce::Colour kPanelBottom { 0xff1b1a20 };
    const juce::Colour kCard        { 0xff232128 };
    const juce::Colour kCardEdge    { 0xff3a3744 };
    const juce::Colour kText        { 0xffe6e1d6 };
    const juce::Colour kTextDim     { 0xff9a95a6 };
    const juce::Colour kAmber       { 0xffffb347 };
    const juce::Colour kLcdGreen    { 0xff8dffa8 };

    juce::Font makeFont (float size, int style = juce::Font::plain)
    {
        return juce::Font (juce::FontOptions (size, style));
    }

    juce::Colour colourForSection (const juce::String& name)
    {
        if (name == "Mix")        return juce::Colour (0xff2f7fc1);
        if (name == "Analog")     return juce::Colour (0xffd23a2e);
        if (name == "Wave/Super") return juce::Colour (0xffe2652a);
        if (name == "FM")         return juce::Colour (0xffc2185b);
        if (name == "Filter")     return juce::Colour (0xffd23a2e);
        if (name == "Amp Env")    return juce::Colour (0xffd99a1d);
        if (name == "Filter Env") return juce::Colour (0xffd99a1d);
        if (name == "LFO")        return juce::Colour (0xff7b4fc9);
        return juce::Colour (0xff2f7fc1);                       // Ensemble, Delay, Reverb
    }

    int rowForSection (const juce::String& name)
    {
        if (name == "Filter" || name == "LFO" || name == "Ensemble" || name == "Delay" || name == "Reverb")
            return 1;
        return 0;
    }

    void paintWood (juce::Graphics& g, juce::Rectangle<int> r, int seed)
    {
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3d1c0c), (float) r.getX(), 0.f,
                                                 juce::Colour (0xff3d1c0c), (float) r.getRight(), 0.f, false));
        g.fillRect (r);

        juce::ColourGradient body (juce::Colour (0xff5a2d14), (float) r.getX(), 0.f,
                                   juce::Colour (0xff5a2d14), (float) r.getRight(), 0.f, false);
        body.addColour (0.5, juce::Colour (0xff8a4f27));
        g.setGradientFill (body);
        g.fillRect (r);

        juce::Random rng (seed);
        for (int i = 0; i < 70; ++i)
        {
            const float x = (float) r.getX() + rng.nextFloat() * (float) r.getWidth();
            const float w = 0.5f + rng.nextFloat() * 1.8f;
            const bool dark = rng.nextBool();
            g.setColour ((dark ? juce::Colour (0xff1e0d05) : juce::Colour (0xffb9733a)).withAlpha (0.05f + rng.nextFloat() * 0.16f));
            g.fillRect (x, (float) r.getY(), w, (float) r.getHeight());
        }

        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.drawRect (r, 1);
    }
}

//==============================================================================
KrakenLookAndFeel::KrakenLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::textColourId, kLcdGreen);
    setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::arrowColourId, kLcdGreen);
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff1b1a20));
    setColour (juce::PopupMenu::textColourId, kText);
    setColour (juce::PopupMenu::headerTextColourId, kAmber);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xffd23a2e));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2e2b36));
    setColour (juce::TextButton::textColourOffId, kAmber);
    setColour (juce::TextButton::textColourOnId, kAmber);
}

juce::Font KrakenLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 19.f, juce::Font::bold));
}

void KrakenLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float startAngle, float endAngle,
                                          juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (3.f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const float angle = startAngle + sliderPos * (endAngle - startAngle);
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    // scale ticks around the knob
    g.setColour (kText.withAlpha (0.55f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.f * (endAngle - startAngle);
        const float r1 = radius, r2 = radius - (i % 5 == 0 ? 4.5f : 3.f);
        g.drawLine (centre.x + std::sin (a) * r1, centre.y - std::cos (a) * r1,
                    centre.x + std::sin (a) * r2, centre.y - std::cos (a) * r2, i % 5 == 0 ? 1.3f : 0.9f);
    }

    // knob body
    const float kr = radius * 0.68f;
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (centre.x - kr + 1.5f, centre.y - kr + 2.5f, kr * 2.f, kr * 2.f);        // drop shadow
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff5a5762), centre.x - kr * 0.4f, centre.y - kr,
                                             juce::Colour (0xff121115), centre.x + kr * 0.3f, centre.y + kr, false));
    g.fillEllipse (centre.x - kr, centre.y - kr, kr * 2.f, kr * 2.f);
    g.setColour (accent.withAlpha (0.8f));
    g.drawEllipse (centre.x - kr, centre.y - kr, kr * 2.f, kr * 2.f, 1.4f);

    // top cap
    const float cr = kr * 0.72f;
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a3842), centre.x, centre.y - cr,
                                             juce::Colour (0xff17161b), centre.x, centre.y + cr, false));
    g.fillEllipse (centre.x - cr, centre.y - cr, cr * 2.f, cr * 2.f);

    // pointer
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.4f, -kr + 1.5f, 2.8f, kr * 0.62f, 1.2f);
    g.setColour (juce::Colours::white);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
}

juce::Label* KrakenLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = juce::LookAndFeel_V4::createSliderTextBox (slider);
    l->setFont (makeFont (9.5f));
    l->setColour (juce::Label::textColourId, kTextDim);
    l->setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    l->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    return l;
}

//==============================================================================
juce::Colour ColourKeyboard::colourFor (int midiNoteNumber)
{
    static const juce::Colour byPitchClass[12] = {
        juce::Colour (0xffe2402f), juce::Colour (0xffe2402f),   // C
        juce::Colour (0xffee7d2b), juce::Colour (0xffee7d2b),   // D
        juce::Colour (0xfff2c230),                              // E
        juce::Colour (0xff4fb85a), juce::Colour (0xff4fb85a),   // F
        juce::Colour (0xff2fa4d6), juce::Colour (0xff2fa4d6),   // G
        juce::Colour (0xff4c6fe0), juce::Colour (0xff4c6fe0),   // A
        juce::Colour (0xff8f55d4)                               // B
    };
    return byPitchClass[((midiNoteNumber % 12) + 12) % 12];
}

void ColourKeyboard::drawWhiteNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                    bool isDown, bool isOver, juce::Colour lineColour, juce::Colour)
{
    const auto accent = colourFor (midiNoteNumber);

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff6f1e4), 0.f, area.getY(),
                                             juce::Colour (0xffd9d2bf), 0.f, area.getBottom(), false));
    g.fillRect (area);

    if (isOver && ! isDown)
    {
        g.setColour (accent.withAlpha (0.18f));
        g.fillRect (area);
    }
    if (isDown)
    {
        g.setColour (accent.withAlpha (0.75f));
        g.fillRect (area);
    }

    // coloured cap at the front edge of the key
    g.setColour (accent);
    g.fillRect (area.getX() + 1.f, area.getBottom() - 11.f, area.getWidth() - 1.f, 9.f);

    g.setColour (lineColour.withAlpha (0.8f));
    g.drawRect (area, 1.f);
}

void ColourKeyboard::drawBlackNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                    bool isDown, bool isOver, juce::Colour)
{
    const auto accent = colourFor (midiNoteNumber);

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff2b2a31), 0.f, area.getY(),
                                             juce::Colour (0xff0c0b0e), 0.f, area.getBottom(), false));
    g.fillRect (area);

    if (isDown || isOver)
    {
        g.setColour (accent.withAlpha (isDown ? 0.8f : 0.25f));
        g.fillRect (area);
    }

    g.setColour (accent.withAlpha (0.9f));
    g.fillRect (area.getX() + 1.f, area.getBottom() - 6.f, area.getWidth() - 2.f, 4.f);
}

//==============================================================================
KrakenEditor::KrakenEditor (KrakenProcessor& p)
    : juce::AudioProcessorEditor (&p),
      processor (p),
      keyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&lookAndFeel);

    // Group the parameters by section, in the order of the registry.
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
            s->colour = colourForSection (name);
            s->row = rowForSection (name);
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
            k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, kCellW - 2, 12);
            k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                           juce::MathConstants<float>::pi * 2.8f, true);
            k->slider.setColour (juce::Slider::rotarySliderFillColourId, section.colour);
            k->slider.setColour (juce::Slider::textBoxTextColourId, kTextDim);
            k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
            addAndMakeVisible (k->slider);

            k->label.setText (juce::String (kParamDefs[id].name).toUpperCase(), juce::dontSendNotification);
            k->label.setJustificationType (juce::Justification::centred);
            k->label.setFont (makeFont (9.f, juce::Font::bold));
            k->label.setColour (juce::Label::textColourId, kText);
            k->label.setInterceptsMouseClicks (false, false);
            addAndMakeVisible (k->label);

            k->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.apvts, kParamDefs[id].id, k->slider);

            knobs[(size_t) id] = std::move (k);
        }
    }

    // Presets, grouped by category
    fillPresetMenu();
    presetBox.setSelectedId (processor.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.setJustificationType (juce::Justification::centred);
    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id > 0)
            processor.setCurrentProgram (id - 1);
    };
    addAndMakeVisible (presetBox);

    prevButton.onClick = [this] { stepPreset (-1); };
    nextButton.onClick = [this] { stepPreset (+1); };
    addAndMakeVisible (prevButton);
    addAndMakeVisible (nextButton);

    // Keyboard along the bottom
    keyboard.setAvailableRange (36, 96);              // C2 .. C7
    keyboard.setScrollButtonsVisible (false);
    keyboard.setVelocity (0.8f, true);
    addAndMakeVisible (keyboard);

    setResizable (false, false);
    setSize (kWidth, kHeight);
    layoutPanel();

    startTimerHz (10);
}

KrakenEditor::~KrakenEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void KrakenEditor::fillPresetMenu()
{
    presetBox.clear (juce::dontSendNotification);
    juce::String lastCategory;
    const auto& list = factoryPresets();
    for (int i = 0; i < (int) list.size(); ++i)
    {
        const juce::String cat (list[(size_t) i].category);
        if (cat != lastCategory)
        {
            presetBox.addSectionHeading (cat.toUpperCase());
            lastCategory = cat;
        }
        presetBox.addItem (juce::String (list[(size_t) i].name), i + 1);
    }
}

void KrakenEditor::layoutPanel()
{
    const int x0 = kCheek + 6;
    const int panelTop = kHeaderH + 8;

    int xs[2] = { x0, x0 };

    for (auto& s : sections)
    {
        const int count = (int) s.params.size();
        const int cols = (count + 1) / 2;
        const int w = cols * kCellW + 2 * kPadX;
        const int x = xs[s.row];
        const int y = panelTop + s.row * (kCardH + kGap);

        s.bounds = { x, y, w, kCardH };

        for (int j = 0; j < count; ++j)
        {
            const int col = j % cols, row = j / cols;
            const int cx = x + kPadX + col * kCellW;
            const int cy = y + kBarH + kPadTop + row * kCellH;
            auto& k = *knobs[(size_t) s.params[(size_t) j]];
            k.label.setBounds (cx, cy, kCellW, kLabelH);
            k.slider.setBounds (cx, cy + kLabelH, kCellW, kCellH - kLabelH);
        }
        xs[s.row] += w + kGap;
    }

    // The second row ends with the preset display, which takes the remaining width.
    displayCard = { xs[1], panelTop + kCardH + kGap, x0 + kInnerW - xs[1], kCardH };
}

void KrakenEditor::resized()
{
    layoutPanel();

    // display: preset name in an LCD window, buttons in the title bar
    prevButton.setBounds (displayCard.getRight() - 56, displayCard.getY() + 2, 24, kBarH - 4);
    nextButton.setBounds (displayCard.getRight() - 30, displayCard.getY() + 2, 24, kBarH - 4);
    lcdBox = { displayCard.getX() + 10, displayCard.getY() + kBarH + 12, displayCard.getWidth() - 20, 62 };
    presetBox.setBounds (lcdBox.reduced (4, 6));

    // keyboard along the bottom
    const int x0 = kCheek + 6;
    keyboardArea = { x0, kHeight - kKeyboardH - 10, kInnerW, kKeyboardH };
    keyboard.setBounds (keyboardArea);
    keyboard.setKeyWidth ((float) kInnerW / 36.f);
}

void KrakenEditor::timerCallback()
{
    const int program = processor.getCurrentProgram();
    if (presetBox.getSelectedId() != program + 1)
        presetBox.setSelectedId (program + 1, juce::dontSendNotification);
}

void KrakenEditor::stepPreset (int delta)
{
    const int n = processor.getNumPrograms();
    int current = presetBox.getSelectedId() - 1;
    if (current < 0) current = processor.getCurrentProgram();
    presetBox.setSelectedId (((current + delta) % n + n) % n + 1, juce::sendNotificationSync);
}

void KrakenEditor::paint (juce::Graphics& g)
{
    // panel
    g.setGradientFill (juce::ColourGradient (kPanelTop, 0.f, 0.f, kPanelBottom, 0.f, (float) kHeight, false));
    g.fillAll();

    // fine brushed-metal lines
    {
        juce::Random rng (7);
        for (int i = 0; i < 160; ++i)
        {
            g.setColour (juce::Colours::white.withAlpha (0.012f + rng.nextFloat() * 0.02f));
            g.fillRect (kCheek, rng.nextInt (kHeight), kWidth - 2 * kCheek, 1);
        }
    }

    // ---- header ----
    {
        g.setColour (juce::Colour (0xff141318));
        g.fillRect (kCheek, 0, kWidth - 2 * kCheek, kHeaderH);

        g.setColour (juce::Colour (0xffd23a2e));
        g.fillRect (kCheek, kHeaderH - 3, kWidth - 2 * kCheek, 3);
        g.setColour (juce::Colour (0xffe2652a));
        g.fillRect (kCheek, kHeaderH - 6, kWidth - 2 * kCheek, 2);
        g.setColour (juce::Colour (0xffd99a1d));
        g.fillRect (kCheek, kHeaderH - 9, kWidth - 2 * kCheek, 1);

        // title
        g.setFont (makeFont (34.f, juce::Font::bold));
        g.setColour (juce::Colour (0xffd23a2e).withAlpha (0.5f));
        g.drawText ("KRAKEN", kCheek + 22, 4, 260, 40, juce::Justification::centredLeft);
        g.setColour (juce::Colours::white);
        g.drawText ("KRAKEN", kCheek + 20, 2, 260, 40, juce::Justification::centredLeft);

        g.setFont (makeFont (10.f, juce::Font::bold));
        g.setColour (kTextDim);
        g.drawText ("16 OSCILLATOR HYBRID POLYSYNTH", kCheek + 22, 36, 320, 12, juce::Justification::centredLeft);

        // voices / oscillators badge on the right
        g.setFont (makeFont (10.f, juce::Font::bold));
        g.setColour (kAmber);
        g.drawText ("8 VOICES  ·  FM + ANALOG + WAVETABLE + SUPERSAW", kWidth - kCheek - 430, 8, 410, 14,
                    juce::Justification::centredRight);
    }

    // ---- section cards ----
    for (const auto& s : sections)
    {
        const auto b = s.bounds.toFloat();
        g.setColour (kCard);
        g.fillRoundedRectangle (b, 4.f);
        g.setColour (kCardEdge);
        g.drawRoundedRectangle (b.reduced (0.5f), 4.f, 1.f);

        // coloured title strip
        juce::Path strip;
        strip.addRoundedRectangle (b.getX(), b.getY(), b.getWidth(), (float) kBarH, 4.f, 4.f, true, true, false, false);
        g.setColour (s.colour);
        g.fillPath (strip);

        g.setFont (makeFont (10.5f, juce::Font::bold));
        g.setColour (juce::Colours::white);
        g.drawText (s.name.toUpperCase(), (int) b.getX() + 8, (int) b.getY(), (int) b.getWidth() - 12, kBarH,
                    juce::Justification::centredLeft);
    }

    // ---- display card ----
    {
        const auto b = displayCard.toFloat();
        g.setColour (kCard);
        g.fillRoundedRectangle (b, 4.f);
        g.setColour (kCardEdge);
        g.drawRoundedRectangle (b.reduced (0.5f), 4.f, 1.f);

        juce::Path strip;
        strip.addRoundedRectangle (b.getX(), b.getY(), b.getWidth(), (float) kBarH, 4.f, 4.f, true, true, false, false);
        g.setColour (juce::Colour (0xffd99a1d));
        g.fillPath (strip);
        g.setFont (makeFont (10.5f, juce::Font::bold));
        g.setColour (juce::Colour (0xff1a1206));
        g.drawText ("PRESET", displayCard.getX() + 8, displayCard.getY(), 100, kBarH, juce::Justification::centredLeft);

        // LCD window
        g.setColour (juce::Colour (0xff050c08));
        g.fillRoundedRectangle (lcdBox.toFloat(), 4.f);
        g.setColour (juce::Colour (0xff1c2a21));
        g.drawRoundedRectangle (lcdBox.toFloat().reduced (0.5f), 4.f, 1.5f);
        g.setColour (kLcdGreen.withAlpha (0.05f));
        for (int yy = lcdBox.getY() + 2; yy < lcdBox.getBottom() - 2; yy += 3)
            g.fillRect (lcdBox.getX() + 2, yy, lcdBox.getWidth() - 4, 1);

        const int program = processor.getCurrentProgram();
        const auto& list = factoryPresets();
        if (program >= 0 && program < (int) list.size())
        {
            g.setFont (makeFont (10.f, juce::Font::bold));
            g.setColour (kLcdGreen.withAlpha (0.7f));
            g.drawText (juce::String (list[(size_t) program].category).toUpperCase() + "   "
                            + juce::String (program + 1).paddedLeft ('0', 2) + "/" + juce::String ((int) list.size()),
                        lcdBox.getX() + 8, lcdBox.getY() + 3, lcdBox.getWidth() - 16, 12, juce::Justification::centredLeft);
        }

        g.setFont (makeFont (9.5f));
        g.setColour (kTextDim);
        g.drawText ("Click the name for the full list", displayCard.getX() + 8, lcdBox.getBottom() + 6,
                    displayCard.getWidth() - 16, 12, juce::Justification::centred);
    }

    // ---- wooden side panels (drawn last so they overlap the panel edges cleanly) ----
    paintWood (g, { 0, 0, kCheek, kHeight }, 11);
    paintWood (g, { kWidth - kCheek, 0, kCheek, kHeight }, 29);

    // keyboard well
    g.setColour (juce::Colour (0xff0d0c10));
    g.fillRect (keyboardArea.expanded (3, 3));
}
