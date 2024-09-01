/*
  ==============================================================================

    VisualPanel.h
    Created: 25 Jun 2022 1:40:51pm
    Author:  Loïc J

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class VisualPanel : public juce::Component
{
public:
    VisualPanel(FTMSynthAudioProcessor& p);
    ~VisualPanel() override;

    void setDimensions(int dim);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FTMSynthAudioProcessor& processor;
    int dimensions;

    Label thisIsALabel;
    Label nameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualPanel)
};
