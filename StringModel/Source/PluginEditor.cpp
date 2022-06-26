/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
StringModelAudioProcessorEditor::StringModelAudioProcessorEditor (StringModelAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), alphaOff(0.25f),
    stringButton("string"), drumButton("drum"), boxButton("box"), gateButton("GATE"),
    mainControls(112, 8, 512, 158), xyzControls(400, 176, 208, 216),
    stringView(p, 1), drumView(p, 2), boxView(p, 3),
    helpComp("help")
{
    setSize (640, 400);

    // Custom look and feel
    setLookAndFeel(new CustomLookAndFeel());


    // Dimension selector
    Image buttons = ImageCache::getFromMemory(BinaryData::dimensions_png, BinaryData::dimensions_pngSize);

    Image buttonOff = buttons.getClippedImage(Rectangle<int> (0, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    Image buttonHovered = buttons.getClippedImage(Rectangle<int> (0, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    Image buttonOn = buttons.getClippedImage(Rectangle<int> (0, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    stringButton.setClickingTogglesState(true);
    stringButton.setImages(true, true, true,
                           buttonOff, 1.0f, Colours::transparentBlack,
                           buttonHovered, 1.0f, Colours::transparentBlack,
                           buttonOn, 1.0f, Colours::transparentBlack,
                           0.0f);
    stringButton.onClick = [this] { setDimensions(1, true); };
    stringButton.setRadioGroupId(1);
    addAndMakeVisible(stringButton);

    buttonOff = buttons.getClippedImage(Rectangle<int> (buttons.getWidth()/3, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonHovered = buttons.getClippedImage(Rectangle<int> (buttons.getWidth()/3, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonOn = buttons.getClippedImage(Rectangle<int> (buttons.getWidth()/3, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    drumButton.setClickingTogglesState(true);
    drumButton.setImages(true, true, true,
                           buttonOff, 1.0f, Colours::transparentBlack,
                           buttonHovered, 1.0f, Colours::transparentBlack,
                           buttonOn, 1.0f, Colours::transparentBlack,
                           0.0f);
    drumButton.onClick = [this] { setDimensions(2, true); };
    drumButton.setRadioGroupId(1);
    addAndMakeVisible(drumButton);

    buttonOff = buttons.getClippedImage(Rectangle<int> (buttons.getWidth()*2/3, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonHovered = buttons.getClippedImage(Rectangle<int> (buttons.getWidth()*2/3, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonOn = buttons.getClippedImage(Rectangle<int> (buttons.getWidth()*2/3, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    boxButton.setClickingTogglesState(true);
    boxButton.setImages(true, true, true,
                           buttonOff, 1.0f, Colours::transparentBlack,
                           buttonHovered, 1.0f, Colours::transparentBlack,
                           buttonOn, 1.0f, Colours::transparentBlack,
                           0.0f);
    boxButton.onClick = [this] { setDimensions(3, true); };
    boxButton.setRadioGroupId(1);
    addAndMakeVisible(boxButton);


    // Knobs and buttons in the top panel
    tauSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    tauSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    tauSlider.setPopupDisplayEnabled(true, true, this);
    // This class maintains a connection between slider and parameter in AudioProcessorValueTreeState
    tauTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "sustain", tauSlider));
    addAndMakeVisible(tauSlider);
    tauLabel.setText("SUSTAIN", dontSendNotification);
    tauLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(tauLabel);

    relSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    relSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    relSlider.setPopupDisplayEnabled(true, true, this);
    relTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "release", relSlider));
    addAndMakeVisible(relSlider);
    relLabel.setText("RELEASE", dontSendNotification);
    relLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(relLabel);

    gateButton.setToggleable(true);
    gateButton.onStateChange = [this] { relSlider.setAlpha(gateButton.getToggleState() ? 1.0 : alphaOff); };
    gateTree.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.tree, "gate", gateButton));
    relSlider.setAlpha(gateButton.getToggleState() ? 1.0 : alphaOff);
    addAndMakeVisible(gateButton);

    pSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    pSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    pSlider.setPopupDisplayEnabled(true, true, this);
    pTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "damp", pSlider));
    addAndMakeVisible(pSlider);
    pLabel.setText("DAMP", dontSendNotification);
    pLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(pLabel);

    dSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    dSlider.setPopupDisplayEnabled(true, true, this);
    dSlider.onValueChange = [this] { updateVisualizations(2); }; // 2D view
    dTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dispersion", dSlider));
    addAndMakeVisible(dSlider);
    dLabel.setText("INHARMONICITY", dontSendNotification);
    dLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(dLabel);

    alpha1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    alpha1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    alpha1Slider.setPopupDisplayEnabled(true, true, this);
    alpha1Slider.onValueChange = [this] { updateVisualizations(6); }; // 2D + 3D view
    alpha1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "squareness", alpha1Slider));
    addAndMakeVisible(alpha1Slider);
    alpha1Label.setText("SQUARENESS", dontSendNotification);
    alpha1Label.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(alpha1Label);

    alpha2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    alpha2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    alpha2Slider.setPopupDisplayEnabled(true, true, this);
    alpha2Slider.onValueChange = [this] { updateVisualizations(4); }; // 3D view
    alpha2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "cubeness", alpha2Slider));
    addAndMakeVisible(alpha2Slider);
    alpha2Label.setText("CUBENESS", dontSendNotification);
    alpha2Label.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(alpha2Label);


    // Knobs in the bottom right panel
    r1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    r1Slider.setPopupDisplayEnabled(true, true, this);
    r1Slider.onValueChange = [this] { updateVisualizations(7); }; // 1D + 2D + 3D view
    r1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "r1", r1Slider));
    addAndMakeVisible(r1Slider);

    r2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    r2Slider.setPopupDisplayEnabled(true, true, this);
    r2Slider.onValueChange = [this] { updateVisualizations(6); }; // 2D + 3D view
    r2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "r2", r2Slider));
    addAndMakeVisible(r2Slider);

    r3Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r3Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    r3Slider.setPopupDisplayEnabled(true, true, this);
    r3Slider.onValueChange = [this] { updateVisualizations(4); }; // 3D view
    r3Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "r3", r3Slider));
    addAndMakeVisible(r3Slider);

    rLabel.setText("IMPULSE", dontSendNotification);
    rLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(rLabel);

    m1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    m1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    m1Slider.setPopupDisplayEnabled(true, true, this);
    m1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "m1", m1Slider));
    addAndMakeVisible(m1Slider);

    m2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    m2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    m2Slider.setPopupDisplayEnabled(true, true, this);
    m2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "m2", m2Slider));
    addAndMakeVisible(m2Slider);

    m3Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    m3Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    m3Slider.setPopupDisplayEnabled(true, true, this);
    m3Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "m3", m3Slider));
    addAndMakeVisible(m3Slider);

    mLabel.setText("MODES", dontSendNotification);
    mLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(mLabel);

    xLabel.setText("X", dontSendNotification);
    xLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(xLabel);

    yLabel.setText("Y", dontSendNotification);
    yLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(yLabel);

    zLabel.setText("Z", dontSendNotification);
    zLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(zLabel);


    // View panels
    addChildComponent(stringView);
    addChildComponent(drumView);
    addChildComponent(boxView);


    // Misc components
    voicesSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    voicesSlider.setLookAndFeel(new DraggableBox());
    voicesSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    voicesTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "voices", voicesSlider));
    addAndMakeVisible(voicesSlider);
    voicesLabel.setText("POLY VOICES", dontSendNotification);
    voicesLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(voicesLabel);

    Image helpImg = ImageCache::getFromMemory(BinaryData::question_png, BinaryData::question_pngSize);
    helpComp.setImage(helpImg);
    helpComp.setTooltip("Information\n This is a drum synth implemented with physical modeling. You may switch between the three available physical models - string, rectangular drum and cuboid box. Each of the knobs controls a combination of the underlying physical parameters, examined based on qualities of the sound produced.\n\nSustain - short to long\nGate - switches to Release on note release\nRelease - short to long\nDamp - resonating to soft\nInharmonicity - harmonic to inharmonic\nSquareness - elongated to square drum\nCubeness - flat to cubic box\n\nYou can also change the playback parameters:\nImpulse X Y Z - the location of the impulse on the string/drum surface/in the box\nModes X Y Z - The number of modes per dimension");
    addAndMakeVisible(&helpComp);


    // Hidden dimension controller
    dimensionsSlider.onValueChange = [this] { setDimensions(dimensionsSlider.getValue(), false); };
    dimTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dimensions", dimensionsSlider));
    addChildComponent(dimensionsSlider);

    setDimensions(processor.tree.getParameterAsValue("dimensions").getValue(), false);
}

StringModelAudioProcessorEditor::~StringModelAudioProcessorEditor()
{
}


void StringModelAudioProcessorEditor::setDimensions(int dimensions, bool btnToSlider)
{
    if (btnToSlider)
    {
        dimensionsSlider.setValue(dimensions);
    }
    else
    {
        if (dimensions == 1)
        {
            stringButton.setToggleState(true, dontSendNotification);
        }
        else if (dimensions == 2)
        {
            drumButton.setToggleState(true, dontSendNotification);
        }
        else if (dimensions == 3)
        {
            boxButton.setToggleState(true, dontSendNotification);
        }
        updateDimensionComponents();
    }
}

void StringModelAudioProcessorEditor::updateDimensionComponents()
{
    int dimensions = dimensionsSlider.getValue();

    if (dimensions == 1)
    {
        alpha1Slider.setAlpha(alphaOff);
        alpha2Slider.setAlpha(alphaOff);
        r2Slider.setAlpha(alphaOff);
        r3Slider.setAlpha(alphaOff);
        m2Slider.setAlpha(alphaOff);
        m3Slider.setAlpha(alphaOff);

        alpha1Label.setAlpha(alphaOff);
        alpha2Label.setAlpha(alphaOff);
        yLabel.setAlpha(alphaOff);
        zLabel.setAlpha(alphaOff);

        drumView.setVisible(false);
        boxView.setVisible(false);
        stringView.setVisible(true);
    }
    else if (dimensions == 2)
    {
        alpha1Slider.setAlpha(1);
        alpha2Slider.setAlpha(alphaOff);
        r2Slider.setAlpha(1);
        r3Slider.setAlpha(alphaOff);
        m2Slider.setAlpha(1);
        m3Slider.setAlpha(alphaOff);

        alpha1Label.setAlpha(1);
        alpha2Label.setAlpha(alphaOff);
        yLabel.setAlpha(1);
        zLabel.setAlpha(alphaOff);

        stringView.setVisible(false);
        boxView.setVisible(false);
        drumView.setVisible(true);
    }
    else if (dimensions == 3)
    {
        alpha1Slider.setAlpha(1);
        alpha2Slider.setAlpha(1);
        r2Slider.setAlpha(1);
        r3Slider.setAlpha(1);
        m2Slider.setAlpha(1);
        m3Slider.setAlpha(1);

        alpha1Label.setAlpha(1);
        alpha2Label.setAlpha(1);
        yLabel.setAlpha(1);
        zLabel.setAlpha(1);

        stringView.setVisible(false);
        drumView.setVisible(false);
        boxView.setVisible(true);
    }
}

void StringModelAudioProcessorEditor::updateVisualizations(int dimensionFlags)
{
    int dimensions = dimensionsSlider.getValue();

    if ((dimensions == 1) && (dimensionFlags & 1))
    {
        stringView.repaint();
    }
    if ((dimensions == 2) && (dimensionFlags & 2))
    {
        drumView.repaint();
    }
    if ((dimensions == 3) && (dimensionFlags & 4))
    {
        boxView.repaint();
    }
}


//==============================================================================
void StringModelAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImage(background, Rectangle<float> (0, 0, 640, 400), RectanglePlacement::stretchToFit);

    // g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)/*.withAlpha(0.5f)*/);
    // g.fillRoundedRectangle(mainControls.getX(), mainControls.getY(), mainControls.getWidth(), mainControls.getHeight(), 4);
    // g.fillRoundedRectangle(xyzControls.getX(), xyzControls.getY(), xyzControls.getWidth(), xyzControls.getHeight(), 4);
    // g.setColour(Colours::black);

    updateDimensionComponents();
}

void StringModelAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    setSize(640, 400);

    // UI components
    // dimensionsSlider.setBounds(0, 0, 128, 16);
    stringButton.setBounds(mainControls.getX() + 256, mainControls.getY() +   8, 64, 40);
    drumButton.setBounds(  mainControls.getX() + 348, mainControls.getY() +   8, 64, 40);
    boxButton.setBounds(   mainControls.getX() + 440, mainControls.getY() +   8, 64, 40);

    tauSlider.setBounds(   mainControls.getX() +   8, mainControls.getY() +  56, 64, 64);
    tauLabel.setBounds(    mainControls.getX() +   6, mainControls.getY() + 128, 72, 14);
    gateButton.setBounds(  mainControls.getX() +  80, mainControls.getY() +  34, 64, 24);
    relSlider.setBounds(   mainControls.getX() +  88, mainControls.getY() +  64, 48, 48);
    relLabel.setBounds(    mainControls.getX() +  90, mainControls.getY() + 124, 48, 14);
    pSlider.setBounds(     mainControls.getX() + 164, mainControls.getY() +  56, 64, 64);
    pLabel.setBounds(      mainControls.getX() + 162, mainControls.getY() + 128, 72, 14);
    dSlider.setBounds(     mainControls.getX() + 256, mainControls.getY() +  56, 64, 64);
    dLabel.setBounds(      mainControls.getX() + 250, mainControls.getY() + 128, 80, 14);
    alpha1Slider.setBounds(mainControls.getX() + 348, mainControls.getY() +  56, 64, 64);
    alpha1Label.setBounds( mainControls.getX() + 346, mainControls.getY() + 128, 72, 14);
    alpha2Slider.setBounds(mainControls.getX() + 440, mainControls.getY() +  56, 64, 64);
    alpha2Label.setBounds( mainControls.getX() + 438, mainControls.getY() + 128, 72, 14);

    r1Slider.setBounds(xyzControls.getX() +   8, xyzControls.getY() +  32, 48, 48);
    r2Slider.setBounds(xyzControls.getX() +  80, xyzControls.getY() +  32, 48, 48);
    r3Slider.setBounds(xyzControls.getX() + 152, xyzControls.getY() +  32, 48, 48);
    rLabel.setBounds(  xyzControls.getX() +  82, xyzControls.getY() +   8, 48, 14);
    m1Slider.setBounds(xyzControls.getX() +   8, xyzControls.getY() + 136, 48, 48);
    m2Slider.setBounds(xyzControls.getX() +  80, xyzControls.getY() + 136, 48, 48);
    m3Slider.setBounds(xyzControls.getX() + 152, xyzControls.getY() + 136, 48, 48);
    mLabel.setBounds(  xyzControls.getX() +  82, xyzControls.getY() + 192, 48, 14);
    xLabel.setBounds(  xyzControls.getX() +  22, xyzControls.getY() + 101, 24, 14);
    yLabel.setBounds(  xyzControls.getX() +  94, xyzControls.getY() + 101, 24, 14);
    zLabel.setBounds(  xyzControls.getX() + 166, xyzControls.getY() + 101, 24, 14);

    voicesSlider.setBounds(16, 16, 76, 24);
    voicesLabel.setBounds( 16, 48, 76, 14);

    // View panels
    drumView.setBounds(  32, 176, 352, 216);
    boxView.setBounds(   32, 176, 352, 216);
    stringView.setBounds(32, 176, 352, 216);

    helpComp.setBounds(32, 344, 48, 48);
}
