/*
  ==============================================================================

    LabelView.cpp
    Created: 13 Oct 2024 5:14:35pm
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

#include "LabelView.h"
#include "../LookAndFeel/CustomLookAndFeel.h"

//==============================================================================
const float is2dAlphaOff[3] = {true, false, false};
const float is3dAlphaOff[3] = {true,  true, false};

//==============================================================================
LabelView::LabelView(FTMSynthAudioProcessor& p)
    : processor(p), hasOpaqueLabels(false)
{
    setSize(640, 400);
    setInterceptsMouseClicks(false, true);

    // Top panel
    volumeLabel.setText("VOLUME", dontSendNotification);
    volumeLabel.setJustificationType(Justification::centred);
    volumeLabel.setTooltip("Main volume");
    addAndMakeVisible(volumeLabel);

    attackLabel.setText("ATTACK", dontSendNotification);
    attackLabel.setJustificationType(Justification::centred);
    attackLabel.setTooltip("Attack intensity");
    addAndMakeVisible(attackLabel);

    pitchLabel.setText("PITCH", dontSendNotification);
    pitchLabel.setJustificationType(Justification::centred);
    pitchLabel.setTooltip("Note pitch");
    addAndMakeVisible(pitchLabel);

    tauLabel.setText("SUSTAIN", dontSendNotification);
    tauLabel.setJustificationType(Justification::centred);
    tauLabel.setTooltip("Duration of the resonance");
    addAndMakeVisible(tauLabel);

    pLabel.setText("DAMP", dontSendNotification);
    pLabel.setJustificationType(Justification::centred);
    pLabel.setTooltip("Damping of the harmonics\n(from resonating to muffled)");
    addAndMakeVisible(pLabel);

    dLabel.setText("INHARMONICITY", dontSendNotification);
    dLabel.setJustificationType(Justification::centred);
    dLabel.setTooltip("Dispersion of the sound waves\n(from harmonic to inharmonic partials)");
    addAndMakeVisible(dLabel);

    alpha1Label.setText("SQUARENESS", dontSendNotification);
    alpha1Label.setJustificationType(Justification::centred);
    alpha1Label.setTooltip("Shape ratio of the 2D drum\n(thin to square)");
    addAndMakeVisible(alpha1Label);

    alpha2Label.setText("CUBENESS", dontSendNotification);
    alpha2Label.setJustificationType(Justification::centred);
    alpha2Label.setTooltip("Shape ratio of the 3D drum\n(thin to cubic)");
    addAndMakeVisible(alpha2Label);

    // Right panel
    rLabel.setText("IMPULSE", dontSendNotification);
    rLabel.setJustificationType(Justification::centred);
    rLabel.setTooltip("Location of the impulse on the\nstring/drum surface/cuboid");
    addAndMakeVisible(rLabel);

    mLabel.setText("MODES", dontSendNotification);
    mLabel.setJustificationType(Justification::centred);
    mLabel.setTooltip("Number of modes (partials)\nper dimension");
    addAndMakeVisible(mLabel);

    xLabel.setText("X", dontSendNotification);
    xLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(xLabel);

    yLabel.setText("Y", dontSendNotification);
    yLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(yLabel);

    zLabel.setText("Z", dontSendNotification);
    zLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(zLabel);

    // Misc components
    voicesLabel.setText("POLY VOICES", dontSendNotification);
    voicesLabel.setJustificationType(Justification::centred);
    voicesLabel.setTooltip("Maximum number of notes\nplaying simultaneously");
    addAndMakeVisible(voicesLabel);

    algoLabel.setText("METHOD", dontSendNotification);
    algoLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(algoLabel);

    // Hidden dimension controller
    dimensionsSlider.onValueChange = [this] { updateDimensionComponents(); };
    dimTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "dimensions", dimensionsSlider));
    addChildComponent(dimensionsSlider);

    updateDimensionComponents();
}

LabelView::~LabelView()
{
}

void LabelView::paint(juce::Graphics&)
{
}

void LabelView::setOpaqueLabels(bool opaqueLabels)
{
    hasOpaqueLabels = opaqueLabels;
    updateDimensionComponents();
}

void LabelView::updateDimensionComponents()
{
    int dimensions = int(dimensionsSlider.getValue());
    float alpha2d = 1;
    float alpha3d = 1;

    if ((dimensions >= 1) && (dimensions <= 3))
    {
        alpha2d = (hasOpaqueLabels ? 1 : (is2dAlphaOff[dimensions - 1] ? alphaOff : 1));
        alpha3d = (hasOpaqueLabels ? 1 : (is3dAlphaOff[dimensions - 1] ? alphaOff : 1));
    }

    alpha1Label.setAlpha(alpha2d);
    yLabel.setAlpha(alpha2d);
    alpha2Label.setAlpha(alpha3d);
    zLabel.setAlpha(alpha3d);
}

void LabelView::resized()
{
    setSize(640, 400);

    // UI components
    int lblOffY = 140;

    volumeLabel.setBounds(mainControls.getX() -  90, mainControls.getY() + lblOffY,      72, 14);
    attackLabel.setBounds(mainControls.getX() -  82, mainControls.getY() + lblOffY + 80, 56, 14);
    pitchLabel.setBounds( mainControls.getX() +  10, mainControls.getY() + lblOffY,      72, 14);
    tauLabel.setBounds(   mainControls.getX() +  94, mainControls.getY() + lblOffY,      72, 14);
    pLabel.setBounds(     mainControls.getX() + 178, mainControls.getY() + lblOffY,      72, 14);
    dLabel.setBounds(     mainControls.getX() + 258, mainControls.getY() + lblOffY,      80, 14);
    alpha1Label.setBounds(mainControls.getX() + 346, mainControls.getY() + lblOffY,      72, 14);
    alpha2Label.setBounds(mainControls.getX() + 430, mainControls.getY() + lblOffY,      72, 14);

    rLabel.setBounds(xyzControls.getX() +  81, xyzControls.getY() +  14, 50, 14);
    mLabel.setBounds(xyzControls.getX() +  81, xyzControls.getY() + 164, 50, 14);
    xLabel.setBounds(xyzControls.getX() +  26, xyzControls.getY() +  89, 24, 14);
    yLabel.setBounds(xyzControls.getX() +  94, xyzControls.getY() +  89, 24, 14);
    zLabel.setBounds(xyzControls.getX() + 162, xyzControls.getY() +  89, 24, 14);

    voicesLabel.setBounds( 18,  48, 76, 14);
    algoLabel.setBounds(  416, 368, 56, 24);
}
