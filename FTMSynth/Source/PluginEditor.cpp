/*
  ==============================================================================

    PluginEditor.cpp
    Created: 10 Oct 2019 10:03:03am
    Authors: Lily H, Loïc J

  ==============================================================================

    This file is part of FTMSynth.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
FTMSynthAudioProcessorEditor::FTMSynthAudioProcessorEditor(FTMSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), alphaOff(0.125f),
    stringButton("string"), drumButton("drum"), boxButton("box"),
    kbTrackButton("KB TRACK"), tauGateButton("RELEASE"), pGateButton("RING"),
    mainControls(112, 8, 512, 158), xyzControls(416, 176, 208, 216),
    visualPanel(p, r1Slider, r2Slider, r3Slider), helpButton("help")
{
    setSize(640, 400);

    // Custom look and feel
    CustomLookAndFeel* customLookAndFeel = new CustomLookAndFeel();
    setLookAndFeel(customLookAndFeel);
    tooltip->setLookAndFeel(customLookAndFeel);


    // Dimension selector
    Image buttons = ImageCache::getFromMemory(BinaryData::dimensions_png, BinaryData::dimensions_pngSize);

    Image buttonOff = buttons.getClippedImage(Rectangle<int>(0, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    Image buttonHovered = buttons.getClippedImage(Rectangle<int>(0, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    Image buttonOn = buttons.getClippedImage(Rectangle<int>(0, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    stringButton.setClickingTogglesState(true);
    stringButton.setImages(false, true, true,
                           buttonOff, 1.0f, Colours::transparentBlack,
                           buttonHovered, 1.0f, Colours::transparentBlack,
                           buttonOn, 1.0f, Colours::transparentBlack,
                           0.0f);
    stringButton.setTooltip("1D model (string)");
    stringButton.onClick = [this] { setDimensions(1, true); };
    stringButton.setRadioGroupId(1);
    addAndMakeVisible(stringButton);

    buttonOff = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()/3, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonHovered = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()/3, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonOn = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()/3, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    drumButton.setClickingTogglesState(true);
    drumButton.setImages(false, true, true,
                         buttonOff, 1.0f, Colours::transparentBlack,
                         buttonHovered, 1.0f, Colours::transparentBlack,
                         buttonOn, 1.0f, Colours::transparentBlack,
                         0.0f);
    drumButton.setTooltip("2D model (drum)");
    drumButton.onClick = [this] { setDimensions(2, true); };
    drumButton.setRadioGroupId(1);
    addAndMakeVisible(drumButton);

    buttonOff = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()*2/3, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonHovered = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()*2/3, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonOn = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()*2/3, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    boxButton.setClickingTogglesState(true);
    boxButton.setImages(false, true, true,
                        buttonOff, 1.0f, Colours::transparentBlack,
                        buttonHovered, 1.0f, Colours::transparentBlack,
                        buttonOn, 1.0f, Colours::transparentBlack,
                        0.0f);
    boxButton.setTooltip("3D model (cuboid)");
    boxButton.onClick = [this] { setDimensions(3, true); };
    boxButton.setRadioGroupId(1);
    addAndMakeVisible(boxButton);


    // Knobs and buttons in the top panel
    volumeSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    volumeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeSlider.setPopupDisplayEnabled(true, true, this);
    // This class maintains a connection between slider and parameter in AudioProcessorValueTreeState
    volumeTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "volume", volumeSlider));
    volumeSlider.textFromValueFunction = [] (double value) { return String(int(value*100)); };
    volumeSlider.setTextValueSuffix(" %");
    addAndMakeVisible(volumeSlider);
    volumeLabel.setText("VOLUME", dontSendNotification);
    volumeLabel.setJustificationType(Justification(Justification::centred));
    volumeLabel.setTooltip("Main volume");
    addAndMakeVisible(volumeLabel);

    pitchSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    pitchSlider.setMouseDragSensitivity(1000); // default is 250
    pitchSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 66, 16);
    pitchSlider.setPopupDisplayEnabled(false, false, this);
    pitchSlider.setLookAndFeel(new WithTextBox());
    pitchTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "pitch", pitchSlider));
    addAndMakeVisible(pitchSlider);
    pitchLabel.setText("PITCH", dontSendNotification);
    pitchLabel.setJustificationType(Justification(Justification::centred));
    pitchLabel.setTooltip("Note pitch");
    addAndMakeVisible(pitchLabel);

    kbTrackButton.setToggleable(true);
    kbTrackTree.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.tree, "kbTrack", kbTrackButton));
    kbTrackButton.setTooltip("Toggles keyboard tracking for note pitch");
    addAndMakeVisible(kbTrackButton);

    tauSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    tauSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    tauSlider.setPopupDisplayEnabled(true, true, this);
    tauSlider.getProperties().set("colour", var(int(0x80FFE600)));
    tauTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "sustain", tauSlider));
    tauSlider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(tauSlider);
    tauLabel.setText("SUSTAIN", dontSendNotification);
    tauLabel.setJustificationType(Justification(Justification::centred));
    tauLabel.setTooltip("Duration of the resonance");
    addAndMakeVisible(tauLabel);

    relSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    relSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    relSlider.setPopupDisplayEnabled(true, true, this);
    relSlider.getProperties().set("colour", var(int(0x80CC8B00)));
    relTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "release", relSlider));
    relSlider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(relSlider);

    tauGateButton.setToggleable(true);
    tauGateButton.onStateChange = [this] { relSlider.setAlpha(tauGateButton.getToggleState() ? 1.0f : alphaOff); };
    tauGateTree.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.tree, "susGate", tauGateButton));
    tauGateButton.setTooltip("Defines whether the volume envelope\nswitches to Release when the key is released");
    relSlider.setAlpha(tauGateButton.getToggleState() ? 1.0f : alphaOff);
    addAndMakeVisible(tauGateButton);

    pSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    pSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    pSlider.setPopupDisplayEnabled(true, true, this);
    pSlider.getProperties().set("colour", var(int(0x80FF6B00)));
    pTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "damp", pSlider));
    pSlider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(pSlider);
    pLabel.setText("DAMP", dontSendNotification);
    pLabel.setJustificationType(Justification(Justification::centred));
    pLabel.setTooltip("Damping of the harmonics\n(from resonating to muffled)");
    addAndMakeVisible(pLabel);

    ringSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    ringSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    ringSlider.setPopupDisplayEnabled(true, true, this);
    ringSlider.getProperties().set("colour", var(int(0x80FF6B00)));
    ringTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "ring", ringSlider));
    ringSlider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(ringSlider);

    pGateButton.setToggleable(true);
    pGateButton.onStateChange = [this] { ringSlider.setAlpha(pGateButton.getToggleState() ? 1.0f : alphaOff); };
    pGateTree.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.tree, "dampGate", pGateButton));
    pGateButton.setTooltip("Defines whether the damping\nswitches to Ring after the key is released");
    ringSlider.setAlpha(pGateButton.getToggleState() ? 1.0f : alphaOff);
    addAndMakeVisible(pGateButton);

    dSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    dSlider.setPopupDisplayEnabled(true, true, this);
    dSlider.getProperties().set("colour", var(int(0x8000EAFF)));
    dTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dispersion", dSlider));
    dSlider.textFromValueFunction = [] (double value) { return String(value, 4); };
    addAndMakeVisible(dSlider);
    dLabel.setText("INHARMONICITY", dontSendNotification);
    dLabel.setJustificationType(Justification(Justification::centred));
    dLabel.setTooltip("Dispersion of the sound waves\n(from harmonic to inharmonic partials)");
    addAndMakeVisible(dLabel);

    alpha1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    alpha1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    alpha1Slider.setPopupDisplayEnabled(true, true, this);
    alpha1Slider.getProperties().set("colour", var(int(0x8000FF44)));
    alpha1Slider.onValueChange = [this] { updateVisualization(true); };
    alpha1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "squareness", alpha1Slider));
    alpha1Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(alpha1Slider);
    alpha1Label.setText("SQUARENESS", dontSendNotification);
    alpha1Label.setJustificationType(Justification(Justification::centred));
    alpha1Label.setTooltip("Shape ratio of the 2D drum\n(thin to square)");
    addAndMakeVisible(alpha1Label);

    alpha2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    alpha2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    alpha2Slider.setPopupDisplayEnabled(true, true, this);
    alpha2Slider.getProperties().set("colour", var(int(0x8067FF00)));
    alpha2Slider.onValueChange = [this] { updateVisualization(true); };
    alpha2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "cubeness", alpha2Slider));
    alpha2Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(alpha2Slider);
    alpha2Label.setText("CUBENESS", dontSendNotification);
    alpha2Label.setJustificationType(Justification(Justification::centred));
    alpha2Label.setTooltip("Shape ratio of the 3D drum\n(thin to cubic)");
    addAndMakeVisible(alpha2Label);


    // Knobs in the bottom right panel
    r1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    r1Slider.setPopupDisplayEnabled(true, true, this);
    r1Slider.onValueChange = [this] { updateVisualization(); };
    r1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "r1", r1Slider));
    r1Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(r1Slider);

    r2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    r2Slider.setPopupDisplayEnabled(true, true, this);
    r2Slider.onValueChange = [this] { updateVisualization(); };
    r2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "r2", r2Slider));
    r2Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(r2Slider);

    r3Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r3Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    r3Slider.setPopupDisplayEnabled(true, true, this);
    r3Slider.onValueChange = [this] { updateVisualization(true); };
    r3Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "r3", r3Slider));
    r3Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(r3Slider);

    rLabel.setText("IMPULSE", dontSendNotification);
    rLabel.setJustificationType(Justification(Justification::centred));
    rLabel.setTooltip("Location of the impulse on the string/drum surface/cuboid");
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
    mLabel.setTooltip("Number of modes (partials) per dimension");
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


    // View panel
    addAndMakeVisible(visualPanel);


    // Misc components
    voicesSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    voicesSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    voicesSlider.setLookAndFeel(new DraggableBox());
    voicesTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "voices", voicesSlider));
    addAndMakeVisible(voicesSlider);
    voicesLabel.setText("POLY VOICES", dontSendNotification);
    voicesLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(voicesLabel);

    algoComboBox.addItemList(StringArray{"IVAN", "RABENSTEIN"}, 1);
    algoComboBox.setEditableText(false);
    algoComboBox.setLookAndFeel(new CustomComboBox());
    algoTree.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(processor.tree, "algorithm", algoComboBox));
    addAndMakeVisible(algoComboBox);
    algoLabel.setText("METHOD", dontSendNotification);
    algoLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(algoLabel);

    Image helpImg = ImageCache::getFromMemory(BinaryData::question_png, BinaryData::question_pngSize);
    Image helpOff = helpImg.getClippedImage(Rectangle<int>(0, 0, helpImg.getWidth()/2, helpImg.getHeight()));
    Image helpHovered = helpImg.getClippedImage(Rectangle<int>(helpImg.getWidth()/2, 0, helpImg.getWidth()/2, helpImg.getHeight()));
    helpButton.setImages(false, true, true,
                         helpOff, 1.0f, Colours::transparentBlack,
                         helpHovered, 1.0f, Colours::transparentBlack,
                         helpHovered, 1.0f, Colours::transparentBlack,
                         0.8f);
    helpButton.setTooltip("Information\n\nThis is a drum synth implemented with physical modeling. You may switch between the three available physical models - string, rectangular drum and cuboid box. Each of the knobs controls a combination of the underlying physical parameters, examined based on qualities of the sound produced.\n\n(Hover the mouse over the knob labels for more informations)");
    addAndMakeVisible(&helpButton);


    // Hidden dimension controller
    dimensionsSlider.onValueChange = [this] { setDimensions(int(dimensionsSlider.getValue()), false); };
    dimTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dimensions", dimensionsSlider));
    addChildComponent(dimensionsSlider);

    setDimensions(processor.tree.getParameterAsValue("dimensions").getValue(), false);
}

FTMSynthAudioProcessorEditor::~FTMSynthAudioProcessorEditor()
{
}


void FTMSynthAudioProcessorEditor::setDimensions(int dimensions, bool btnToSlider)
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

void FTMSynthAudioProcessorEditor::updateDimensionComponents()
{
    int dimensions = int(dimensionsSlider.getValue());

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
    }

    visualPanel.setDimensions(dimensions);
    updateVisualization();
}

void FTMSynthAudioProcessorEditor::updateVisualization(bool updateBounds)
{
    if (updateBounds) visualPanel.updateBounds();
    visualPanel.repaint();
}


//==============================================================================
void FTMSynthAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImage(background, Rectangle<float>(0, 0, 640, 400), RectanglePlacement::stretchToFit);

    updateDimensionComponents();
}

void FTMSynthAudioProcessorEditor::resized()
{
    setSize(640, 400);

    // UI components
    int btnOffY = 16;
    int knobOffY = 72;
    int toggleOffY = 52;
    int knob2OffY = 4;
    int lblOffY = knobOffY + 68;

    stringButton.setBounds( mainControls.getX() + 266, mainControls.getY() +    btnOffY, 64, 40);
    drumButton.setBounds(   mainControls.getX() + 350, mainControls.getY() +    btnOffY, 64, 40);
    boxButton.setBounds(    mainControls.getX() + 434, mainControls.getY() +    btnOffY, 64, 40);

    volumeSlider.setBounds( mainControls.getX() -  88, mainControls.getY() +   knobOffY, 64, 64);
    volumeLabel.setBounds(  mainControls.getX() -  90, mainControls.getY() +    lblOffY, 72, 14);
    // attackSlider.setBounds( mainControls.getX() -  80, mainControls.getY() +   knobOffY, 48, 48);
    // attackLabel.setBounds(  mainControls.getX() -  82, mainControls.getY() +   knobOffY, 56, 14);

    pitchSlider.setBounds(  mainControls.getX() +  14, mainControls.getY() +   knobOffY-16, 64, 64+16);
    pitchLabel.setBounds(   mainControls.getX() +  10, mainControls.getY() +    lblOffY, 72, 14);
    kbTrackButton.setBounds(mainControls.getX() +  10, mainControls.getY() + toggleOffY-24, 72, 24);

    tauSlider.setBounds(    mainControls.getX() +  98, mainControls.getY() +   knobOffY, 64, 64);
    tauLabel.setBounds(     mainControls.getX() +  94, mainControls.getY() +    lblOffY, 72, 14);
    tauGateButton.setBounds(mainControls.getX() +  98, mainControls.getY() + toggleOffY, 64, 24);
    relSlider.setBounds(    mainControls.getX() + 106, mainControls.getY() +  knob2OffY, 48, 48);
    pSlider.setBounds(      mainControls.getX() + 182, mainControls.getY() +   knobOffY, 64, 64);
    pLabel.setBounds(       mainControls.getX() + 178, mainControls.getY() +    lblOffY, 72, 14);
    pGateButton.setBounds(  mainControls.getX() + 182, mainControls.getY() + toggleOffY, 64, 24);
    ringSlider.setBounds(   mainControls.getX() + 190, mainControls.getY() +  knob2OffY, 48, 48);

    dSlider.setBounds(      mainControls.getX() + 266, mainControls.getY() +   knobOffY, 64, 64);
    dLabel.setBounds(       mainControls.getX() + 258, mainControls.getY() +    lblOffY, 80, 14);
    alpha1Slider.setBounds( mainControls.getX() + 350, mainControls.getY() +   knobOffY, 64, 64);
    alpha1Label.setBounds(  mainControls.getX() + 346, mainControls.getY() +    lblOffY, 72, 14);
    alpha2Slider.setBounds( mainControls.getX() + 434, mainControls.getY() +   knobOffY, 64, 64);
    alpha2Label.setBounds(  mainControls.getX() + 430, mainControls.getY() +    lblOffY, 72, 14);

    r1Slider.setBounds(xyzControls.getX() +  12, xyzControls.getY() +  34, 48, 48);
    r2Slider.setBounds(xyzControls.getX() +  80, xyzControls.getY() +  34, 48, 48);
    r3Slider.setBounds(xyzControls.getX() + 148, xyzControls.getY() +  34, 48, 48);
    rLabel.setBounds(  xyzControls.getX() +  81, xyzControls.getY() +  16, 50, 14);
    m1Slider.setBounds(xyzControls.getX() +  12, xyzControls.getY() + 110, 48, 48);
    m2Slider.setBounds(xyzControls.getX() +  80, xyzControls.getY() + 110, 48, 48);
    m3Slider.setBounds(xyzControls.getX() + 148, xyzControls.getY() + 110, 48, 48);
    mLabel.setBounds(  xyzControls.getX() +  81, xyzControls.getY() + 164, 50, 14);
    xLabel.setBounds(  xyzControls.getX() +  26, xyzControls.getY() +  89, 24, 14);
    yLabel.setBounds(  xyzControls.getX() +  94, xyzControls.getY() +  89, 24, 14);
    zLabel.setBounds(  xyzControls.getX() + 162, xyzControls.getY() +  89, 24, 14);

    voicesSlider.setBounds(16, 16, 80, 24);
    voicesLabel.setBounds( 18, 48, 76, 14);

    visualPanel.setBounds(16, 180, 352, 212);
    helpButton.setBounds(16, 344, 48, 48);
    // midiButton.setBounds(52, 344, 48, 48);

    algoComboBox.setBounds(480, 368, 144, 24);
    algoLabel.setBounds(   416, 368,  56, 24);
}
