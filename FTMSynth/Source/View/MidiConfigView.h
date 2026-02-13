/*
  ==============================================================================

    MidiConfigView.h
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

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "../LookAndFeel/CustomLookAndFeel.h"
#include "MidiConfigButton.h"

//==============================================================================
const Rectangle<int> configControls(128, 184, 256, 208);

//==============================================================================
class MidiConfigView : public Component, public ChangeListener
{
public:
    MidiConfigView(FTMSynthAudioProcessor& p);
    ~MidiConfigView() override;

    void paint(Graphics&) override;
    void resized() override;

    // ChangeListener callback
    void changeListenerCallback(ChangeBroadcaster* source) override;

    void createDefaultSliderListener();

private:
    void updateView(int button_id = 0);
    void updateAllButtons();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FTMSynthAudioProcessor& processor;

    // Custom look-and-feel
    CustomLookAndFeel customLookAndFeel;
    DraggableBox draggableBox;
    FunnyFont funnyFont;

    // Help tooltip
    SharedResourcePointer<TooltipWindow> tooltip;

    // Buttons
    MidiConfigButton volumeButton;
    MidiConfigButton attackButton;
    MidiConfigButton dimensionsButton;
    MidiConfigButton pitchButton;
    MidiConfigButton kbTrackButton;
    MidiConfigButton tauButton;
    MidiConfigButton tauGateButton;
    MidiConfigButton relButton;
    MidiConfigButton pButton;
    MidiConfigButton pGateButton;
    MidiConfigButton ringButton;
    MidiConfigButton dButton;
    MidiConfigButton alpha1Button;
    MidiConfigButton alpha2Button;
    MidiConfigButton r1Button;
    MidiConfigButton r2Button;
    MidiConfigButton r3Button;
    MidiConfigButton m1Button;
    MidiConfigButton m2Button;
    MidiConfigButton m3Button;
    MidiConfigButton voicesButton;
    MidiConfigButton algoButton;

    int current_button_id = 0;
    MidiConfigButton* midiConfigButtons[numMappableParams] = {
        &volumeButton,
        &attackButton,
        &dimensionsButton,
        &pitchButton,
        &kbTrackButton,
        &tauButton,
        &tauGateButton,
        &relButton,
        &pButton,
        &pGateButton,
        &ringButton,
        &dButton,
        &alpha1Button,
        &alpha2Button,
        &r1Button,
        &r2Button,
        &r3Button,
        &m1Button,
        &m2Button,
        &m3Button,
        &voicesButton,
        &algoButton
    };

    // Extra labels
    Label kbTrackLabel;
    Label relLabel;
    Label ringLabel;
    Label gateLabel;
    Label dimensionsLabel;

    // Center panel UI components
    Label configLabel;
    Label paramNameLabel;

    Label midiCCLabel;
    Label midiChannelLabel;
    Label midiDefaultLabel;

    Slider midiCCSlider;
    Slider midiChannelSlider;
    Slider midiDefaultSlider;

    DrawableButton resetCCButton;
    DrawableButton resetChannelButton;
    DrawableButton learnCCButton;
    DrawableButton learnChannelButton;

    ImageButton mappingFileButton;

    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiConfigView)
};
