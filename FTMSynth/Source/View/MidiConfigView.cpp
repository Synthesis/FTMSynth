/*
  ==============================================================================

    MidiConfigView.cpp
    Created: 4 Oct 2024 12:03:56am
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

#include "MidiConfigView.h"
#include "../LookAndFeel/CustomLookAndFeel.h"
#include "BinaryData.h"

//==============================================================================
static String getParamID(int buttonID)
{
    switch (buttonID)
    {
        case BUTTON_ID_VOLUME:     return "volume";
        case BUTTON_ID_ATTACK:     return "attack";
        case BUTTON_ID_DIMENSIONS: return "dimensions";
        case BUTTON_ID_PITCH:      return "pitch";
        case BUTTON_ID_KBTRACK:    return "kbTrack";
        case BUTTON_ID_TAU:        return "sustain";
        case BUTTON_ID_TAU_GATE:   return "susGate";
        case BUTTON_ID_RELEASE:    return "release";
        case BUTTON_ID_P:          return "damp";
        case BUTTON_ID_P_GATE:     return "dampGate";
        case BUTTON_ID_RING:       return "ring";
        case BUTTON_ID_D:          return "dispersion";
        case BUTTON_ID_ALPHA1:     return "squareness";
        case BUTTON_ID_ALPHA2:     return "cubeness";
        case BUTTON_ID_R1:         return "r1";
        case BUTTON_ID_R2:         return "r2";
        case BUTTON_ID_R3:         return "r3";
        case BUTTON_ID_M1:         return "m1";
        case BUTTON_ID_M2:         return "m2";
        case BUTTON_ID_M3:         return "m3";
        case BUTTON_ID_VOICES:     return "voices";
        case BUTTON_ID_ALGO:       return "algorithm";
        default: return "";
    }
}

//==============================================================================
MidiConfigView::MidiConfigView(FTMSynthAudioProcessor& p)
    : processor(p),
      resetCCButton("resetCC", DrawableButton::ImageOnButtonBackgroundOriginalSize),
      resetChannelButton("resetChannel", DrawableButton::ImageOnButtonBackgroundOriginalSize),
      learnCCButton("learnCC", DrawableButton::ImageOnButtonBackgroundOriginalSize),
      learnChannelButton("learnChannel", DrawableButton::ImageOnButtonBackgroundOriginalSize)
{
    setSize(640, 400);
    setInterceptsMouseClicks(false, true);

    // Parameter selectors
    volumeButton.onClick = [this] { updateView(BUTTON_ID_VOLUME); };
    volumeButton.setRadioGroupId(2);
    addAndMakeVisible(volumeButton);

    attackButton.onClick = [this] { updateView(BUTTON_ID_ATTACK); };
    attackButton.setRadioGroupId(2);
    addAndMakeVisible(attackButton);

    dimensionsButton.onClick = [this] { updateView(BUTTON_ID_DIMENSIONS); };
    dimensionsButton.setRadioGroupId(2);
    addAndMakeVisible(dimensionsButton);

    pitchButton.onClick = [this] { updateView(BUTTON_ID_PITCH); };
    pitchButton.setRadioGroupId(2);
    pitchButton.setConnectedEdges(Button::ConnectedOnTop);
    addAndMakeVisible(pitchButton);

    kbTrackButton.onClick = [this] { updateView(BUTTON_ID_KBTRACK); };
    kbTrackButton.setRadioGroupId(2);
    kbTrackButton.setConnectedEdges(Button::ConnectedOnBottom);
    addAndMakeVisible(kbTrackButton);

    tauButton.onClick = [this] { updateView(BUTTON_ID_TAU); };
    tauButton.setRadioGroupId(2);
    tauButton.setConnectedEdges(Button::ConnectedOnTop);
    addAndMakeVisible(tauButton);

    tauGateButton.onClick = [this] { updateView(BUTTON_ID_TAU_GATE); };
    tauGateButton.setRadioGroupId(2);
    tauGateButton.setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnBottom);
    addAndMakeVisible(tauGateButton);

    relButton.onClick = [this] { updateView(BUTTON_ID_RELEASE); };
    relButton.setRadioGroupId(2);
    relButton.setConnectedEdges(Button::ConnectedOnBottom);
    addAndMakeVisible(relButton);

    pButton.onClick = [this] { updateView(BUTTON_ID_P); };
    pButton.setRadioGroupId(2);
    pButton.setConnectedEdges(Button::ConnectedOnTop);
    addAndMakeVisible(pButton);

    pGateButton.onClick = [this] { updateView(BUTTON_ID_P_GATE); };
    pGateButton.setRadioGroupId(2);
    pGateButton.setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnBottom);
    addAndMakeVisible(pGateButton);

    ringButton.onClick = [this] { updateView(BUTTON_ID_RING); };
    ringButton.setRadioGroupId(2);
    ringButton.setConnectedEdges(Button::ConnectedOnBottom);
    addAndMakeVisible(ringButton);

    dButton.onClick = [this] { updateView(BUTTON_ID_D); };
    dButton.setRadioGroupId(2);
    addAndMakeVisible(dButton);

    alpha1Button.onClick = [this] { updateView(BUTTON_ID_ALPHA1); };
    alpha1Button.setRadioGroupId(2);
    addAndMakeVisible(alpha1Button);

    alpha2Button.onClick = [this] { updateView(BUTTON_ID_ALPHA2); };
    alpha2Button.setRadioGroupId(2);
    addAndMakeVisible(alpha2Button);

    r1Button.onClick = [this] { updateView(BUTTON_ID_R1); };
    r1Button.setRadioGroupId(2);
    addAndMakeVisible(r1Button);

    r2Button.onClick = [this] { updateView(BUTTON_ID_R2); };
    r2Button.setRadioGroupId(2);
    addAndMakeVisible(r2Button);

    r3Button.onClick = [this] { updateView(BUTTON_ID_R3); };
    r3Button.setRadioGroupId(2);
    addAndMakeVisible(r3Button);

    m1Button.onClick = [this] { updateView(BUTTON_ID_M1); };
    m1Button.setRadioGroupId(2);
    addAndMakeVisible(m1Button);

    m2Button.onClick = [this] { updateView(BUTTON_ID_M2); };
    m2Button.setRadioGroupId(2);
    addAndMakeVisible(m2Button);

    m3Button.onClick = [this] { updateView(BUTTON_ID_M3); };
    m3Button.setRadioGroupId(2);
    addAndMakeVisible(m3Button);

    voicesButton.onClick = [this] { updateView(BUTTON_ID_VOICES); };
    voicesButton.setRadioGroupId(2);
    addAndMakeVisible(voicesButton);

    algoButton.onClick = [this] { updateView(BUTTON_ID_ALGO); };
    algoButton.setRadioGroupId(2);
    addAndMakeVisible(algoButton);

    kbTrackLabel.setText("KB TRACK", dontSendNotification);
    kbTrackLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(kbTrackLabel);

    relLabel.setText("RELEASE", dontSendNotification);
    relLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(relLabel);

    ringLabel.setText("RING", dontSendNotification);
    ringLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(ringLabel);

    gateLabel.setText("GATE", dontSendNotification);
    gateLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(gateLabel);

    dimensionsLabel.setText("DIMENSIONS", dontSendNotification);
    dimensionsLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(dimensionsLabel);

    // Config panel
    configLabel.setLookAndFeel(&funnyFont);
    configLabel.setText("midi cc\nmapping.", dontSendNotification);
    configLabel.setJustificationType(Justification::topLeft);
    configLabel.setColour(Label::textColourId, Colour(0xFF5F5F5F));
    addAndMakeVisible(configLabel);

    paramNameLabel.setText("(NONE)", dontSendNotification);
    paramNameLabel.setJustificationType(Justification::centred);
    addChildComponent(paramNameLabel);

    // CC selector
    DrawableImage resetDrawable(ImageCache::getFromMemory(BinaryData::midiReset_png, BinaryData::midiReset_pngSize));
    Image midiLearnImage = ImageCache::getFromMemory(BinaryData::midiLearn_png, BinaryData::midiLearn_pngSize);
    DrawableImage midiLearnDrawable(midiLearnImage.getClippedImage(Rectangle<int>(0, 0, midiLearnImage.getWidth()/2, midiLearnImage.getHeight())));
    DrawableImage midiLearnDrawableOn(midiLearnImage.getClippedImage(Rectangle<int>(midiLearnImage.getWidth()/2, 0, midiLearnImage.getWidth()/2, midiLearnImage.getHeight())));

    midiCCLabel.setText("CC", dontSendNotification);
    midiCCLabel.setJustificationType(Justification::centredRight);
    addChildComponent(midiCCLabel);

    midiCCSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    midiCCSlider.setMouseDragSensitivity(500); // default is 250
    midiCCSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    midiCCSlider.setLookAndFeel(&draggableBox);
    midiCCSlider.setRange(-1, 127, 1);
    midiCCSlider.textFromValueFunction = [](double value)
    {
        if (value == -1) return String("OFF");
        return String(int(value));
    };
    midiCCSlider.onValueChange = [this]
    {
        if (current_button_id > 0)
        {
            MidiMappingEntry* entry = processor.midiMappings[getParamID(current_button_id)].get();
            *entry->cc = (int)midiCCSlider.getValue(); // Manual update
            midiConfigButtons[current_button_id-1]->setMapping(entry->cc->get(), entry->channel->get());
            if (!isDragging) // Save on click/scroll
                processor.saveGlobalMidiMappings();
        }
    };
    midiCCSlider.onDragStart = [this] { isDragging = true; };
    midiCCSlider.onDragEnd = [this]
    {
        isDragging = false;
        processor.saveGlobalMidiMappings();
    };
    addChildComponent(midiCCSlider);

    resetCCButton.setImages(&resetDrawable);
    resetCCButton.setTooltip("Reset");
    resetCCButton.setWantsKeyboardFocus(false);
    resetCCButton.onClick = [this]
    {
        if (current_button_id > 0)
        {
            midiCCSlider.setValue(-1, sendNotification);
        }
    };
    addChildComponent(resetCCButton);

    learnCCButton.setImages(&midiLearnDrawable, nullptr, nullptr, nullptr, &midiLearnDrawableOn);
    learnCCButton.setTooltip("MIDI Learn");
    learnCCButton.setClickingTogglesState(true);
    learnCCButton.setColour(TextButton::buttonOnColourId, Colour(0xFFFC5647));
    learnCCButton.setWantsKeyboardFocus(false);
    learnCCButton.onClick = [this]
    {
        if (current_button_id > 0)
            processor.setMidiLearn(getParamID(current_button_id), true, false);
        else
            learnCCButton.setToggleState(false, dontSendNotification);
    };
    addChildComponent(learnCCButton);

    // Channel selector
    midiChannelLabel.setText("CHANNEL", dontSendNotification);
    midiChannelLabel.setJustificationType(Justification::centredRight);
    addChildComponent(midiChannelLabel);

    midiChannelSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    midiChannelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    midiChannelSlider.setLookAndFeel(&draggableBox);
    midiChannelSlider.setRange(-2, 15, 1);
    midiChannelSlider.textFromValueFunction = [](double value)
    {
        if (value == -2) return String("MAIN");
        if (value == -1) return String("OMNI");
        return String(int(value) + 1);
    };
    midiChannelSlider.onValueChange = [this]
    {
        if (current_button_id > 0)
        {
            MidiMappingEntry* entry = processor.midiMappings[getParamID(current_button_id)].get();
            *entry->channel = (int)midiChannelSlider.getValue(); // Manual update
            midiConfigButtons[current_button_id-1]->setMapping(entry->cc->get(), entry->channel->get());
            if (!isDragging)
                processor.saveGlobalMidiMappings();
        }
    };
    midiChannelSlider.onDragStart = [this] { isDragging = true; };
    midiChannelSlider.onDragEnd = [this]
    {
        isDragging = false;
        processor.saveGlobalMidiMappings();
    };
    addChildComponent(midiChannelSlider);

    resetChannelButton.setImages(&resetDrawable);
    resetChannelButton.setTooltip("Reset");
    resetChannelButton.setWantsKeyboardFocus(false);
    resetChannelButton.onClick = [this]
    {
        if (current_button_id > 0)
        {
            midiChannelSlider.setValue(-2, sendNotification);
        }
    };
    addChildComponent(resetChannelButton);

    learnChannelButton.setImages(&midiLearnDrawable, nullptr, nullptr, nullptr, &midiLearnDrawableOn);
    learnChannelButton.setTooltip("MIDI Learn");
    learnChannelButton.setClickingTogglesState(true);
    learnChannelButton.setColour(TextButton::buttonOnColourId, Colour(0xFFFC5647));
    learnChannelButton.setWantsKeyboardFocus(false);
    learnChannelButton.onClick = [this]
    {
        if (current_button_id > 0)
            processor.setMidiLearn(getParamID(current_button_id), false, true);
        else
            learnChannelButton.setToggleState(false, dontSendNotification);
    };
    addChildComponent(learnChannelButton);

    // Default channel selector
    midiDefaultLabel.setText("DEFAULT CHN", dontSendNotification);
    midiDefaultLabel.setJustificationType(Justification::centredRight);
    addAndMakeVisible(midiDefaultLabel);

    midiDefaultSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    midiDefaultSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    midiDefaultSlider.setLookAndFeel(&draggableBox);
    midiDefaultSlider.setRange(-1, 15, 1);
    midiDefaultSlider.textFromValueFunction = [](double value)
    {
        if (value == -1) return String("OMNI");
        return String(int(value) + 1);
    };
    midiDefaultSlider.onValueChange = [this]
    {
        *processor.defaultChannelParam = (int)midiDefaultSlider.getValue(); // Manual update
        if (!isDragging)
            processor.saveGlobalMidiMappings();
    };
    midiDefaultSlider.onDragStart = [this] { isDragging = true; };
    midiDefaultSlider.onDragEnd = [this]
    {
        isDragging = false;
        processor.saveGlobalMidiMappings();
    };
    // Manual initial sync
    midiDefaultSlider.setValue(processor.defaultChannelParam->get(), dontSendNotification);
    addAndMakeVisible(midiDefaultSlider);

    // Initialize buttons with current processor state
    updateAllButtons();

    Image mappingImg = ImageCache::getFromMemory(BinaryData::midiMapping_png, BinaryData::midiMapping_pngSize);
    Image loadOff = mappingImg.getClippedImage(Rectangle<int>(0, 0, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    Image loadHovered = mappingImg.getClippedImage(Rectangle<int>(mappingImg.getWidth()/3, 0, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    Image loadDown = mappingImg.getClippedImage(Rectangle<int>(mappingImg.getWidth()*2/3, 0, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    loadButton.setImages(false, true, true,
                         loadOff, 1.0f, Colours::transparentBlack,
                         loadHovered, 1.0f, Colours::transparentBlack,
                         loadDown, 1.0f, Colours::transparentBlack,
                         0.8f);
    loadButton.setTooltip("Load MIDI mapping");
    loadButton.onClick = [this]
    {
        auto fc = std::make_shared<FileChooser>("Load MIDI configuration",
                                                File::getSpecialLocation(File::userHomeDirectory),
                                                "*.xml");

        fc->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                        [this, fc](const FileChooser& chooser)
                        {
                            File result = chooser.getResult();
                            if (result != File{})
                            {
                                XmlDocument doc(result);
                                if (auto xml = doc.getDocumentElement())
                                {
                                    processor.restoreMidiMappingsFromXml(*xml);
                                    processor.saveGlobalMidiMappings();
                                    updateAllButtons();
                                    updateView();

                                    // Update Manual Default Channel Slider
                                    midiDefaultSlider.setValue(processor.defaultChannelParam->get(), dontSendNotification);
                                }
                            }
                        });
    };
    addAndMakeVisible(loadButton);

    Image saveOff = mappingImg.getClippedImage(Rectangle<int>(0, mappingImg.getHeight()/3, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    Image saveHovered = mappingImg.getClippedImage(Rectangle<int>(mappingImg.getWidth()/3, mappingImg.getHeight()/3, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    Image saveDown = mappingImg.getClippedImage(Rectangle<int>(mappingImg.getWidth()*2/3, mappingImg.getHeight()/3, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    saveButton.setImages(false, true, true,
                         saveOff, 1.0f, Colours::transparentBlack,
                         saveHovered, 1.0f, Colours::transparentBlack,
                         saveDown, 1.0f, Colours::transparentBlack,
                         0.8f);
    saveButton.setTooltip("Save MIDI mapping");
    saveButton.onClick = [this]
    {
        auto fc = std::make_shared<FileChooser>("Save MIDI configuration",
                                                File::getSpecialLocation(File::userHomeDirectory),
                                                "*.xml");

        fc->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting,
                        [this, fc](const FileChooser& chooser)
                        {
                            File result = chooser.getResult();
                            if (result != File{})
                            {
                                // Ensure extension
                                if (!result.hasFileExtension(".xml"))
                                    result = result.withFileExtension(".xml");

                                if (auto xml = processor.getMidiMappingsAsXml())
                                {
                                    xml->writeTo(result);
                                }
                            }
                        });
    };
    addAndMakeVisible(saveButton);

    Image resetOff = mappingImg.getClippedImage(Rectangle<int>(0, mappingImg.getHeight()*2/3, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    Image resetHovered = mappingImg.getClippedImage(Rectangle<int>(mappingImg.getWidth()/3, mappingImg.getHeight()*2/3, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    Image resetDown = mappingImg.getClippedImage(Rectangle<int>(mappingImg.getWidth()*2/3, mappingImg.getHeight()*2/3, mappingImg.getWidth()/3, mappingImg.getHeight()/3));
    resetButton.setImages(false, true, true,
                         resetOff, 1.0f, Colours::transparentBlack,
                         resetHovered, 1.0f, Colours::transparentBlack,
                         resetDown, 1.0f, Colours::transparentBlack,
                         0.8f);
    resetButton.setTooltip("Reset MIDI mapping");
    resetButton.onClick = [this]
    {
        NativeMessageBox::showYesNoBox(AlertWindow::QuestionIcon,
            "Reset MIDI configuration",
            "Reset all MIDI mapping?\nThis operation cannot be undone.",
            this,
            ModalCallbackFunction::create([this](int result)
            {
                if (result != 0)  // Yes
                {
                    // Reset all mappings
                    for (auto const& [id, entry] : processor.midiMappings)
                    {
                        *entry->cc = -1;       // OFF
                        *entry->channel = -2;  // MAIN
                    }
                    *processor.defaultChannelParam = -1;  // OMNI

                    processor.saveGlobalMidiMappings();
                    
                    updateAllButtons();
                    updateView();
                    midiDefaultSlider.setValue(-1, dontSendNotification);
                }
            }));
    };
    addAndMakeVisible(resetButton);

    processor.addChangeListener(this);
}

MidiConfigView::~MidiConfigView()
{
    processor.removeChangeListener(this);
}

void MidiConfigView::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &processor)
    {
        // Update learn buttons state
        learnCCButton.setToggleState(processor.learningCC, dontSendNotification);
        learnChannelButton.setToggleState(processor.learningChannel, dontSendNotification);

        if (current_button_id > 0)
        {
            MidiMapping mapping = processor.getMidiMapping(getParamID(current_button_id));

            if ((int)midiCCSlider.getValue() != mapping.cc)
                midiCCSlider.setValue(mapping.cc, dontSendNotification);

            if ((int)midiChannelSlider.getValue() != mapping.channel)
                midiChannelSlider.setValue(mapping.channel, dontSendNotification);

            midiConfigButtons[current_button_id-1]->setMapping(mapping.cc, mapping.channel);
        }
    }
}

void MidiConfigView::updateView(int button_id)
{
    // Ensure learn mode is reset when switching params
    learnCCButton.setToggleState(false, sendNotification);
    learnChannelButton.setToggleState(false, sendNotification);
    processor.setMidiLearn(getParamID(current_button_id), false, false);

    if (button_id > 0)
    {
        if (current_button_id == button_id)
        {
            midiConfigButtons[current_button_id-1]->setToggleState(false, dontSendNotification);
            current_button_id = 0;

            paramNameLabel.setText("(NONE)", dontSendNotification);

            if (paramNameLabel.isVisible()) paramNameLabel.setVisible(false);
            if (midiCCLabel.isVisible()) midiCCLabel.setVisible(false);
            if (midiCCSlider.isVisible()) midiCCSlider.setVisible(false);
            if (resetCCButton.isVisible()) resetCCButton.setVisible(false);
            if (learnCCButton.isVisible()) learnCCButton.setVisible(false);
            if (midiChannelLabel.isVisible()) midiChannelLabel.setVisible(false);
            if (midiChannelSlider.isVisible()) midiChannelSlider.setVisible(false);
            if (resetChannelButton.isVisible()) resetChannelButton.setVisible(false);
            if (learnChannelButton.isVisible()) learnChannelButton.setVisible(false);
        }
        else
        {
            current_button_id = button_id;
            // Manual sync
            String paramID = getParamID(current_button_id);
            MidiMappingEntry* entry = processor.midiMappings[paramID].get();
            midiCCSlider.setValue(entry->cc->get(), dontSendNotification);
            midiChannelSlider.setValue(entry->channel->get(), dontSendNotification);

            paramNameLabel.setText(paramName[button_id-1], dontSendNotification);

            if (!paramNameLabel.isVisible()) paramNameLabel.setVisible(true);
            if (!midiCCLabel.isVisible()) midiCCLabel.setVisible(true);
            if (!midiCCSlider.isVisible()) midiCCSlider.setVisible(true);
            if (!resetCCButton.isVisible()) resetCCButton.setVisible(true);
            if (!learnCCButton.isVisible()) learnCCButton.setVisible(true);
            if (!midiChannelLabel.isVisible()) midiChannelLabel.setVisible(true);
            if (!midiChannelSlider.isVisible()) midiChannelSlider.setVisible(true);
            if (!resetChannelButton.isVisible()) resetChannelButton.setVisible(true);
            if (!learnChannelButton.isVisible()) learnChannelButton.setVisible(true);

            // Button update
            midiConfigButtons[current_button_id-1]->setMapping((int)midiCCSlider.getValue(), (int)midiChannelSlider.getValue());
        }
    }

    repaint();
}

void MidiConfigView::updateAllButtons()
{
    for (int i=0; i < numElementsInArray(midiConfigButtons); i++)
    {
        if (MidiConfigButton* btn = midiConfigButtons[i])
        {
            String paramID = getParamID(i + 1);
            if (MidiMappingEntry* entry = processor.midiMappings[paramID].get())
            {
                btn->setMapping(entry->cc->get(), entry->channel->get());
            }
        }
    }
}

void MidiConfigView::paint(Graphics& g)
{
    if (!current_button_id)
    {
        g.setColour(Colour(0x7F000000));
        g.drawMultiLineText("Select a control\nto change its MIDI mapping",
                            configControls.getX(),
                            configControls.getCentreY() - 40,
                            configControls.getWidth(),
                            Justification::centred);
    }
}

void MidiConfigView::resized()
{
    setSize(640, 400);
    int knobOffY = 88;
    int lblOffY = 5;

    volumeButton.setBounds(    mainControls.getX()    -  80, mainControls.getY() + knobOffY,      48, 48);
    attackButton.setBounds(    mainControls.getX()    -  80, mainControls.getY() + knobOffY + 80, 48, 48);
    dimensionsButton.setBounds(mainControls.getRight() - 70, mainControls.getY() + knobOffY - 64, 48, 48);
    dimensionsLabel.setBounds( mainControls.getRight() - 86, mainControls.getY() +  lblOffY,      80, 14);
    pitchButton.setBounds(     mainControls.getX()    +  22, mainControls.getY() + knobOffY,      48, 48);
    kbTrackButton.setBounds(   mainControls.getX()    +  22, mainControls.getY() + knobOffY - 32, 48, 32);
    kbTrackLabel.setBounds(    mainControls.getX()    +  10, mainControls.getY() +  lblOffY + 32, 72, 14);
    tauButton.setBounds(       mainControls.getX()    + 106, mainControls.getY() + knobOffY,      48, 48);
    tauGateButton.setBounds(   mainControls.getX()    + 106, mainControls.getY() + knobOffY - 32, 48, 32);
    relButton.setBounds(       mainControls.getX()    + 106, mainControls.getY() + knobOffY - 64, 48, 32);
    relLabel.setBounds(        mainControls.getX()    +  94, mainControls.getY() +  lblOffY,      72, 14);
    pButton.setBounds(         mainControls.getX()    + 190, mainControls.getY() + knobOffY,      48, 48);
    pGateButton.setBounds(     mainControls.getX()    + 190, mainControls.getY() + knobOffY - 32, 48, 32);
    ringButton.setBounds(      mainControls.getX()    + 190, mainControls.getY() + knobOffY - 64, 48, 32);
    ringLabel.setBounds(       mainControls.getX()    + 178, mainControls.getY() +  lblOffY,      72, 14);
    gateLabel.setBounds(       mainControls.getX()    + 160, mainControls.getY() +  lblOffY + 60, 24, 14);
    dButton.setBounds(         mainControls.getX()    + 274, mainControls.getY() + knobOffY,      48, 48);
    alpha1Button.setBounds(    mainControls.getX()    + 358, mainControls.getY() + knobOffY,      48, 48);
    alpha2Button.setBounds(    mainControls.getX()    + 442, mainControls.getY() + knobOffY,      48, 48);

    r1Button.setBounds(xyzControls.getX() +  12, xyzControls.getY() +  34, 48, 48);
    r2Button.setBounds(xyzControls.getX() +  80, xyzControls.getY() +  34, 48, 48);
    r3Button.setBounds(xyzControls.getX() + 148, xyzControls.getY() +  34, 48, 48);
    m1Button.setBounds(xyzControls.getX() +  12, xyzControls.getY() + 110, 48, 48);
    m2Button.setBounds(xyzControls.getX() +  80, xyzControls.getY() + 110, 48, 48);
    m3Button.setBounds(xyzControls.getX() + 148, xyzControls.getY() + 110, 48, 48);

    voicesButton.setBounds(16,  16,  80, 24);
    algoButton.setBounds( 488, 368, 136, 24);

    configLabel.setBounds(32, 278, 96, 64);

    paramNameLabel.setBounds(    configControls.getCentreX() -  48, configControls.getY()            + 25, 96, 14);
    midiCCLabel.setBounds(       configControls.getCentreX() - 128, configControls.getCentreY() - 23 - 12, 96, 14);
    resetCCButton.setBounds(     configControls.getCentreX() -  24, configControls.getCentreY() - 28 - 12, 24, 24);
    midiCCSlider.setBounds(      configControls.getCentreX() +   4, configControls.getCentreY() - 28 - 12, 64, 24);
    learnCCButton.setBounds(     configControls.getCentreX() +  72, configControls.getCentreY() - 28 - 12, 24, 24);
    midiChannelLabel.setBounds(  configControls.getCentreX() - 128, configControls.getCentreY() +  9 - 12, 96, 14);
    resetChannelButton.setBounds(configControls.getCentreX() -  24, configControls.getCentreY() +  4 - 12, 24, 24);
    midiChannelSlider.setBounds( configControls.getCentreX() +   4, configControls.getCentreY() +  4 - 12, 64, 24);
    learnChannelButton.setBounds(configControls.getCentreX() +  72, configControls.getCentreY() +  4 - 12, 24, 24);
    midiDefaultLabel.setBounds(  configControls.getCentreX() - 100, configControls.getCentreY() + 41 - 12, 96, 14);
    midiDefaultSlider.setBounds( configControls.getCentreX() +   4, configControls.getCentreY() + 36 - 12, 64, 24);

    loadButton.setBounds( configControls.getCentreX() - 64, 344, 48, 48);
    saveButton.setBounds( configControls.getCentreX() - 24, 344, 48, 48);
    resetButton.setBounds(configControls.getCentreX() + 16, 344, 48, 48);
}
