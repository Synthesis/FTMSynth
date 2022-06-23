/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Customize.h"

//==============================================================================
class StringModelAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    StringModelAudioProcessorEditor (StringModelAudioProcessor&);
    ~StringModelAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void setDimensions(int dimensions);
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StringModelAudioProcessor& processor;

    SharedResourcePointer <TooltipWindow> help;

    Customize stringCUS;
    Customize drumCUS;
    Customize boxCUS;

    DrawableButton stringButton;
    DrawableButton drumButton;
    DrawableButton boxButton;
    Slider dimensionsSlider;

    Image background;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dimSelection;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringModelAudioProcessorEditor)
};
