/*
  ==============================================================================

    MainView.cpp
    Created: 4 Oct 2024 12:03:18am
    Author:  Loïc J

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

#include "MainView.h"

//==============================================================================
MainView::MainView(FTMSynthAudioProcessor& p)
    : processor(p),
      stringButton("string"), drumButton("drum"), boxButton("box"),
      kbTrackButton("KB TRACK"), tauGateButton("RELEASE"), pGateButton("RING"),
      visualPanel(p.tree, r1Slider, r2Slider, r3Slider)
{
    setSize(640, 400);
    setInterceptsMouseClicks(false, true);

    // Dimension selector
    Image buttons = ImageCache::getFromMemory(BinaryData::dimensions_png, BinaryData::dimensions_pngSize);

    Image buttonOff = buttons.getClippedImage(Rectangle<int>(0, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    Image buttonHovered = buttons.getClippedImage(Rectangle<int>(0, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    Image buttonOn = buttons.getClippedImage(Rectangle<int>(0, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    stringButton.setImages(false, true, true,
                           buttonOff, 1.0f, Colours::transparentBlack,
                           buttonHovered, 1.0f, Colours::transparentBlack,
                           buttonOn, 1.0f, Colours::transparentBlack,
                           0.0f);
    stringButton.setClickingTogglesState(true);
    stringButton.setTooltip("1D model (string)");
    stringButton.onClick = [this] { setDimensions(1, true); };
    stringButton.setRadioGroupId(1);
    addAndMakeVisible(stringButton);

    buttonOff = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()/3, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonHovered = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()/3, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonOn = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()/3, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    drumButton.setImages(false, true, true,
                         buttonOff, 1.0f, Colours::transparentBlack,
                         buttonHovered, 1.0f, Colours::transparentBlack,
                         buttonOn, 1.0f, Colours::transparentBlack,
                         0.0f);
    drumButton.setClickingTogglesState(true);
    drumButton.setTooltip("2D model (drum)");
    drumButton.onClick = [this] { setDimensions(2, true); };
    drumButton.setRadioGroupId(1);
    addAndMakeVisible(drumButton);

    buttonOff = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()*2/3, 0, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonHovered = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()*2/3, buttons.getHeight()/3, buttons.getWidth()/3, buttons.getHeight()/3));
    buttonOn = buttons.getClippedImage(Rectangle<int>(buttons.getWidth()*2/3, buttons.getHeight()*2/3, buttons.getWidth()/3, buttons.getHeight()/3));
    boxButton.setImages(false, true, true,
                        buttonOff, 1.0f, Colours::transparentBlack,
                        buttonHovered, 1.0f, Colours::transparentBlack,
                        buttonOn, 1.0f, Colours::transparentBlack,
                        0.0f);
    boxButton.setClickingTogglesState(true);
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

    pitchSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    pitchSlider.setMouseDragSensitivity(1000); // default is 250
    pitchSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 66, 16);
    pitchSlider.setPopupDisplayEnabled(false, false, this);
    pitchSlider.setLookAndFeel(&withTextBox);
    pitchTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "pitch", pitchSlider));
    addAndMakeVisible(pitchSlider);

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
    tauGateButton.setTooltip("Defines whether the volume envelope\nswitches to Release when the note\nis released");
    relSlider.setAlpha(tauGateButton.getToggleState() ? 1.0f : alphaOff);
    addAndMakeVisible(tauGateButton);

    pSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    pSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    pSlider.setPopupDisplayEnabled(true, true, this);
    pSlider.getProperties().set("colour", var(int(0x80FF6B00)));
    pTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "damp", pSlider));
    pSlider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(pSlider);

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
    pGateButton.setTooltip("Defines whether the damping\nswitches to Ring when the note\nis released");
    ringSlider.setAlpha(pGateButton.getToggleState() ? 1.0f : alphaOff);
    addAndMakeVisible(pGateButton);

    dSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    dSlider.setPopupDisplayEnabled(true, true, this);
    dSlider.getProperties().set("colour", var(int(0x8000EAFF)));
    dTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dispersion", dSlider));
    dSlider.textFromValueFunction = [] (double value) { return String(value, 4); };
    addAndMakeVisible(dSlider);

    alpha1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    alpha1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    alpha1Slider.setPopupDisplayEnabled(true, true, this);
    alpha1Slider.getProperties().set("colour", var(int(0x8000FF44)));
    alpha1Slider.onValueChange = [this] { updateVisualization(true); };
    alpha1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "squareness", alpha1Slider));
    alpha1Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(alpha1Slider);

    alpha2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    alpha2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    alpha2Slider.setPopupDisplayEnabled(true, true, this);
    alpha2Slider.getProperties().set("colour", var(int(0x8067FF00)));
    alpha2Slider.onValueChange = [this] { updateVisualization(true); };
    alpha2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "cubeness", alpha2Slider));
    alpha2Slider.textFromValueFunction = [] (double value) { return String(value, 3); };
    addAndMakeVisible(alpha2Slider);

    // Knobs in the right panel
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


    // View panel
    thisIsALabel.setLookAndFeel(&funnyFont);
    thisIsALabel.setText("this is\na", dontSendNotification);
    thisIsALabel.setJustificationType(Justification::topLeft);
    thisIsALabel.setColour(Label::textColourId, Colour(0xFF5F5F5F));
    addAndMakeVisible(thisIsALabel);
    nameLabel.setLookAndFeel(&funnyFont);
    nameLabel.setJustificationType(Justification::topLeft);
    addAndMakeVisible(nameLabel);

    addAndMakeVisible(visualPanel);

    aboutLabel.setLookAndFeel(&funnyFont);
    aboutLabel.setText("about\nftmsynth.", dontSendNotification);
    aboutLabel.setJustificationType(Justification::topLeft);
    aboutLabel.setColour(Label::textColourId, Colour(0xFF5F5F5F));
    addChildComponent(aboutLabel);

    addChildComponent(helpPanel);


    // Needs to be placed over the view panel in order to be clickable
    attackSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    attackSlider.setPopupDisplayEnabled(true, true, this);
    attackSlider.onValueChange = [this] {
        attackSlider.setAlpha((attackSlider.getValue() == attackSlider.getMaximum()) ? alphaOff : 1);
    };
    attackTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "attack", attackSlider));
    attackSlider.textFromValueFunction = [] (double value) { return String(int(value*100)); };
    attackSlider.setTextValueSuffix(" %");
    addAndMakeVisible(attackSlider);


    // Misc components
    voicesSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    voicesSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    voicesSlider.setLookAndFeel(&draggableBox);
    voicesTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "voices", voicesSlider));
    addAndMakeVisible(voicesSlider);

    algoComboBox.addItemList(StringArray{"IVAN", "RABENSTEIN"}, 1);
    algoComboBox.setEditableText(false);
    algoComboBox.setLookAndFeel(&customComboBox);
    algoTree.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(processor.tree, "algorithm", algoComboBox));
    addAndMakeVisible(algoComboBox);


    // Hidden dimension controller
    dimensionsSlider.onValueChange = [this] { setDimensions(int(dimensionsSlider.getValue()), false); };
    dimTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dimensions", dimensionsSlider));
    addChildComponent(dimensionsSlider);

    setDimensions(processor.tree.getParameterAsValue("dimensions").getValue(), false);
}

MainView::~MainView()
{
    pitchSlider.setLookAndFeel(nullptr);
    voicesSlider.setLookAndFeel(nullptr);
    algoComboBox.setLookAndFeel(nullptr);
}

void MainView::showHelp(bool show)
{
    if (show)
    {
        thisIsALabel.setVisible(false);
        nameLabel.setVisible(false);
        visualPanel.setVisible(false);
        aboutLabel.setVisible(true);
        helpPanel.setVisible(true);
    }
    else
    {
        aboutLabel.setVisible(false);
        helpPanel.setVisible(false);
        thisIsALabel.setVisible(true);
        nameLabel.setVisible(true);
        visualPanel.setVisible(true);
    }
}

void MainView::setDimensions(int dimensions, bool btnToSlider)
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

void MainView::updateDimensionComponents()
{
    int dimensions = int(dimensionsSlider.getValue());
    String strObj("");

    if (dimensions == 1)
    {
        alpha1Slider.setAlpha(alphaOff);
        alpha2Slider.setAlpha(alphaOff);
        r2Slider.setAlpha(alphaOff);
        r3Slider.setAlpha(alphaOff);
        m2Slider.setAlpha(alphaOff);
        m3Slider.setAlpha(alphaOff);

        strObj += "string.";
    }
    else if (dimensions == 2)
    {
        alpha1Slider.setAlpha(1);
        alpha2Slider.setAlpha(alphaOff);
        r2Slider.setAlpha(1);
        r3Slider.setAlpha(alphaOff);
        m2Slider.setAlpha(1);
        m3Slider.setAlpha(alphaOff);

        strObj += "drum.";
    }
    else if (dimensions == 3)
    {
        alpha1Slider.setAlpha(1);
        alpha2Slider.setAlpha(1);
        r2Slider.setAlpha(1);
        r3Slider.setAlpha(1);
        m2Slider.setAlpha(1);
        m3Slider.setAlpha(1);

        strObj += "cuboid.";
    }

    nameLabel.setText(strObj, dontSendNotification);

    visualPanel.setDimensions(dimensions);
    updateVisualization();
}

void MainView::updateVisualization(bool updateMouseBounds)
{
    if (updateMouseBounds) visualPanel.updateMouseBounds();
    visualPanel.repaint();
}

void MainView::paint(juce::Graphics&)
{
    updateDimensionComponents();
}

void MainView::resized()
{
    setSize(640, 400);

    // UI components
    int btnOffY = 16;
    int knobOffY = 72;
    int toggleOffY = 52;
    int knob2OffY = 4;

    stringButton.setBounds( mainControls.getX() + 266, mainControls.getY() +    btnOffY,      64, 40);
    drumButton.setBounds(   mainControls.getX() + 350, mainControls.getY() +    btnOffY,      64, 40);
    boxButton.setBounds(    mainControls.getX() + 434, mainControls.getY() +    btnOffY,      64, 40);

    volumeSlider.setBounds( mainControls.getX() -  88, mainControls.getY() +   knobOffY,      64, 64);
    attackSlider.setBounds( mainControls.getX() -  80, mainControls.getY() +   knobOffY + 96, 48, 48);

    pitchSlider.setBounds(  mainControls.getX() +  14, mainControls.getY() +   knobOffY - 16, 64, 64 + 16);
    kbTrackButton.setBounds(mainControls.getX() +  10, mainControls.getY() + toggleOffY - 24, 72, 24);

    tauSlider.setBounds(    mainControls.getX() +  98, mainControls.getY() +   knobOffY,      64, 64);
    tauGateButton.setBounds(mainControls.getX() +  98, mainControls.getY() + toggleOffY,      64, 24);
    relSlider.setBounds(    mainControls.getX() + 106, mainControls.getY() +  knob2OffY,      48, 48);
    pSlider.setBounds(      mainControls.getX() + 182, mainControls.getY() +   knobOffY,      64, 64);
    pGateButton.setBounds(  mainControls.getX() + 182, mainControls.getY() + toggleOffY,      64, 24);
    ringSlider.setBounds(   mainControls.getX() + 190, mainControls.getY() +  knob2OffY,      48, 48);

    dSlider.setBounds(      mainControls.getX() + 266, mainControls.getY() +   knobOffY,      64, 64);
    alpha1Slider.setBounds( mainControls.getX() + 350, mainControls.getY() +   knobOffY,      64, 64);
    alpha2Slider.setBounds( mainControls.getX() + 434, mainControls.getY() +   knobOffY,      64, 64);

    r1Slider.setBounds(xyzControls.getX() +  12, xyzControls.getY() +  34, 48, 48);
    r2Slider.setBounds(xyzControls.getX() +  80, xyzControls.getY() +  34, 48, 48);
    r3Slider.setBounds(xyzControls.getX() + 148, xyzControls.getY() +  34, 48, 48);
    m1Slider.setBounds(xyzControls.getX() +  12, xyzControls.getY() + 110, 48, 48);
    m2Slider.setBounds(xyzControls.getX() +  80, xyzControls.getY() + 110, 48, 48);
    m3Slider.setBounds(xyzControls.getX() + 148, xyzControls.getY() + 110, 48, 48);

    thisIsALabel.setBounds(  32,      278,  96,  64);
    nameLabel.setBounds(32 + 20, 278 + 28,  80,  40);
    visualPanel.setBounds(  152,      176, 224, 224);
    aboutLabel.setBounds(    32,      278, 128,  64);
    helpPanel.setBounds(    116,      188, 280, 200);

    voicesSlider.setBounds( 16,  16,  80, 24);
    algoComboBox.setBounds(488, 368, 136, 24);
}
