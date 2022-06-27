/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "VisualPanel.h"

//==============================================================================
class StringModelAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    StringModelAudioProcessorEditor (StringModelAudioProcessor&);
    ~StringModelAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    void setDimensions(int dimensions, bool btnToSlider);
    void updateDimensionComponents();
    void updateVisualizations(int dimensionFlags);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StringModelAudioProcessor& processor;

    // UI components
    Slider dimensionsSlider;
    ImageButton stringButton;
    ImageButton drumButton;
    ImageButton boxButton;

    float alphaOff;

    Slider tauSlider;
    ToggleButton gateButton;
    Slider relSlider;
    Slider pSlider;
    Slider dSlider;
    Slider alpha1Slider;
    Slider alpha2Slider;
    Slider r1Slider;
    Slider r2Slider;
    Slider r3Slider;
    Slider m1Slider;
    Slider m2Slider;
    Slider m3Slider;
    Slider voicesSlider;

    VisualPanel stringView;
    VisualPanel drumView;
    VisualPanel boxView;

    // Labels
    Label tauLabel;
    Label relLabel;
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

    Rectangle<int> mainControls;
    Rectangle<int> xyzControls;

    // Attachments from model to components
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dimTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> tauTree;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> gateTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> relTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> pTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> alpha1Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> alpha2Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> r1Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> r2Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> r3Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m1Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m2Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m3Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> voicesTree;

    // Help tooltip
    ImageButton helpButton;
    SharedResourcePointer<TooltipWindow> tooltip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringModelAudioProcessorEditor)
};
