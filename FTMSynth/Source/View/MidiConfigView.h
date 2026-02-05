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
#define BUTTON_ID_VOLUME      1
#define BUTTON_ID_ATTACK      2
#define BUTTON_ID_DIMENSIONS  3
#define BUTTON_ID_PITCH       4
#define BUTTON_ID_KBTRACK     5
#define BUTTON_ID_TAU         6
#define BUTTON_ID_TAU_GATE    7
#define BUTTON_ID_RELEASE     8
#define BUTTON_ID_P           9
#define BUTTON_ID_P_GATE     10
#define BUTTON_ID_RING       11
#define BUTTON_ID_D          12
#define BUTTON_ID_ALPHA1     13
#define BUTTON_ID_ALPHA2     14
#define BUTTON_ID_R1         15
#define BUTTON_ID_R2         16
#define BUTTON_ID_R3         17
#define BUTTON_ID_M1         18
#define BUTTON_ID_M2         19
#define BUTTON_ID_M3         20
#define BUTTON_ID_VOICES     21
#define BUTTON_ID_ALGO       22

//==============================================================================
const Rectangle<int> configControls(128, 184, 256, 208);

//==============================================================================
class MidiConfigView : public juce::Component, public ChangeListener
{
public:
    MidiConfigView(FTMSynthAudioProcessor& p);
    ~MidiConfigView() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // ChangeListener callback
    void changeListenerCallback(ChangeBroadcaster* source) override;

    void createDefaultSliderListener();

private:
    void updateView(int button_id);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FTMSynthAudioProcessor& processor;

    // Custom look-and-feel
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
    MidiConfigButton* midiConfigButtons[22] = {
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

    String paramName[22] = {
        "VOLUME",
        "ATTACK",
        "DIMENSIONS",
        "PITCH",
        "KB TRACK",
        "SUSTAIN",
        "SUSTAIN GATE",
        "RELEASE",
        "DAMP",
        "DAMP GATE",
        "RING",
        "INHARMONICITY",
        "SQUARENESS",
        "CUBENESS",
        "IMPULSE X",
        "IMPULSE Y",
        "IMPULSE Z",
        "MODES X",
        "MODES Y",
        "MODES Z",
        "POLY VOICES",
        "ALGORITHM"
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

    ImageButton loadButton;
    ImageButton saveButton;
    ImageButton resetButton;

    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiConfigView)
};
