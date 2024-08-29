/*
  ==============================================================================

    PluginEditor.h
    Created: 10 Oct 2019 10:03:03am
    Authors: Lily H, Loïc J

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "VisualPanel.h"

//==============================================================================
class FTMSynthAudioProcessorEditor : public AudioProcessorEditor
{
public:
    FTMSynthAudioProcessorEditor(FTMSynthAudioProcessor&);
    ~FTMSynthAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    void setDimensions(int dimensions, bool btnToSlider);
    void updateDimensionComponents();
    void updateVisualizations(int dimensionFlags);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FTMSynthAudioProcessor& processor;

    // UI components
    Slider dimensionsSlider;
    ImageButton stringButton;
    ImageButton drumButton;
    ImageButton boxButton;

    float alphaOff;

    Slider volumeSlider;
    Slider pitchSlider;
    ToggleButton kbTrackButton;
    Slider tauSlider;
    ToggleButton tauGateButton;
    Slider relSlider;
    Slider pSlider;
    ToggleButton pGateButton;
    Slider ringSlider;
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
    ComboBox algoComboBox;

    VisualPanel stringView;
    VisualPanel drumView;
    VisualPanel boxView;

    // Labels
    Label volumeLabel;
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

    Rectangle<int> mainControls;
    Rectangle<int> xyzControls;

    // Attachments from model to components
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dimTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volumeTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> pitchTree;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> kbTrackTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> tauTree;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> tauGateTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> relTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> pTree;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> pGateTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ringTree;
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
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> algoTree;

    // Help tooltip
    ImageButton helpButton;
    SharedResourcePointer<TooltipWindow> tooltip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FTMSynthAudioProcessorEditor)
};
