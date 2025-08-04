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
#include "MidiConfigButton.h"

//==============================================================================
class MidiConfigView : public juce::Component
{
public:
    MidiConfigView(FTMSynthAudioProcessor& p);
    ~MidiConfigView() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FTMSynthAudioProcessor& processor;

    // Help tooltip
    SharedResourcePointer<TooltipWindow> tooltip;

    // Buttons
    MidiConfigButton volumeButton;
    MidiConfigButton attackButton;
    MidiConfigButton dimensionsButton;
    MidiConfigButton pitchButton;
    MidiConfigButton tauButton;
    MidiConfigButton pButton;
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

    // Extra label
    Label dimensionsLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiConfigView)
};
