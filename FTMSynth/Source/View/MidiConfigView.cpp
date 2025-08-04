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

//==============================================================================
MidiConfigView::MidiConfigView(FTMSynthAudioProcessor& p)
    : processor(p)
{
    setSize(640, 400);
    setInterceptsMouseClicks(false, true);

    // Buttons
    volumeButton.onClick = [this] { /* TODO */ };
    volumeButton.setRadioGroupId(2);
    addAndMakeVisible(volumeButton);

    attackButton.onClick = [this] { /* TODO */ };
    attackButton.setRadioGroupId(2);
    addAndMakeVisible(attackButton);

    dimensionsButton.onClick = [this] { /* TODO */ };
    dimensionsButton.setRadioGroupId(2);
    addAndMakeVisible(dimensionsButton);

    pitchButton.onClick = [this] { /* TODO */ };
    pitchButton.setRadioGroupId(2);
    addAndMakeVisible(pitchButton);

    tauButton.onClick = [this] { /* TODO */ };
    tauButton.setRadioGroupId(2);
    addAndMakeVisible(tauButton);

    pButton.onClick = [this] { /* TODO */ };
    pButton.setRadioGroupId(2);
    addAndMakeVisible(pButton);

    dButton.onClick = [this] { /* TODO */ };
    dButton.setRadioGroupId(2);
    addAndMakeVisible(dButton);

    alpha1Button.onClick = [this] { /* TODO */ };
    alpha1Button.setRadioGroupId(2);
    addAndMakeVisible(alpha1Button);

    alpha2Button.onClick = [this] { /* TODO */ };
    alpha2Button.setRadioGroupId(2);
    addAndMakeVisible(alpha2Button);

    r1Button.onClick = [this] { /* TODO */ };
    r1Button.setRadioGroupId(2);
    addAndMakeVisible(r1Button);

    r2Button.onClick = [this] { /* TODO */ };
    r2Button.setRadioGroupId(2);
    addAndMakeVisible(r2Button);

    r3Button.onClick = [this] { /* TODO */ };
    r3Button.setRadioGroupId(2);
    addAndMakeVisible(r3Button);

    m1Button.onClick = [this] { /* TODO */ };
    m1Button.setRadioGroupId(2);
    addAndMakeVisible(m1Button);

    m2Button.onClick = [this] { /* TODO */ };
    m2Button.setRadioGroupId(2);
    addAndMakeVisible(m2Button);

    m3Button.onClick = [this] { /* TODO */ };
    m3Button.setRadioGroupId(2);
    addAndMakeVisible(m3Button);

    voicesButton.onClick = [this] { /* TODO */ };
    voicesButton.setRadioGroupId(2);
    addAndMakeVisible(voicesButton);

    algoButton.onClick = [this] { /* TODO */ };
    algoButton.setRadioGroupId(2);
    addAndMakeVisible(algoButton);

    dimensionsLabel.setText("DIMENSIONS", dontSendNotification);
    dimensionsLabel.setJustificationType(Justification(Justification::centred));
    addAndMakeVisible(dimensionsLabel);
}

MidiConfigView::~MidiConfigView()
{
}

void MidiConfigView::paint(juce::Graphics&)
{
}

void MidiConfigView::resized()
{
    setSize(640, 400);
    int knobOffY = 88;
    int lblOffY = 5;

    volumeButton.setBounds(    mainControls.getX() -  80, mainControls.getY() + knobOffY,      48, 48);
    attackButton.setBounds(    mainControls.getX() -  80, mainControls.getY() + knobOffY + 80, 48, 48);
    dimensionsButton.setBounds(mainControls.getRight() - 70, mainControls.getY() + knobOffY - 64, 48, 48);
    dimensionsLabel.setBounds( mainControls.getRight() - 86, mainControls.getY() +  lblOffY,      80, 14);
    pitchButton.setBounds(     mainControls.getX() +  22, mainControls.getY() + knobOffY - 32, 48, 48 + 32);
    tauButton.setBounds(       mainControls.getX() + 106, mainControls.getY() + knobOffY - 48, 48, 48 + 48);
    pButton.setBounds(         mainControls.getX() + 190, mainControls.getY() + knobOffY - 48, 48, 48 + 48);
    dButton.setBounds(         mainControls.getX() + 274, mainControls.getY() + knobOffY,      48, 48);
    alpha1Button.setBounds(    mainControls.getX() + 358, mainControls.getY() + knobOffY,      48, 48);
    alpha2Button.setBounds(    mainControls.getX() + 442, mainControls.getY() + knobOffY,      48, 48);

    r1Button.setBounds(xyzControls.getX() +  12, xyzControls.getY() +  34, 48, 48);
    r2Button.setBounds(xyzControls.getX() +  80, xyzControls.getY() +  34, 48, 48);
    r3Button.setBounds(xyzControls.getX() + 148, xyzControls.getY() +  34, 48, 48);
    m1Button.setBounds(xyzControls.getX() +  12, xyzControls.getY() + 110, 48, 48);
    m2Button.setBounds(xyzControls.getX() +  80, xyzControls.getY() + 110, 48, 48);
    m3Button.setBounds(xyzControls.getX() + 148, xyzControls.getY() + 110, 48, 48);

    voicesButton.setBounds( 16,  16,  80, 24);
    algoButton.setBounds(480, 368, 144, 24);
}
