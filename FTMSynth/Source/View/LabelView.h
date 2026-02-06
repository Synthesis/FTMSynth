/*
  ==============================================================================

    LabelView.h
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

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"

//==============================================================================
class LabelView : public Component
{
public:
    LabelView(FTMSynthAudioProcessor& p);
    ~LabelView() override;

    void paint(Graphics&) override;
    void resized() override;

    void setOpaqueLabels(bool opaqueLabels);

private:
    void updateDimensionComponents();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FTMSynthAudioProcessor& processor;

    // Help tooltip
    SharedResourcePointer<TooltipWindow> tooltip;

    // Labels
    Label volumeLabel;
    Label attackLabel;
    Label pitchLabel;
    Label tauLabel;
    Label pLabel;
    Label dLabel;
    Label alpha1Label;
    Label alpha2Label;
    Label rLabel;
    Label mLabel;
    Label xLabel;
    Label yLabel;
    Label zLabel;
    Label voicesLabel;
    Label algoLabel;

    // Used for label opacity
    Slider dimensionsSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dimTree;
    bool hasOpaqueLabels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelView)
};
