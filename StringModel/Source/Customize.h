/*
  ==============================================================================

    Customize.h
    Created: 10 Oct 2019 10:02:42am
    Author:  Lily H

  ==============================================================================
*/
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "cusSlider.h"
//#include "mytoolwindow.h"
//==============================================================================
/*
 */
class Customize : public Component
{
public:
    Customize(StringModelAudioProcessor&,int mode);
    ~Customize();

    void paint (Graphics&) override;
    void resized() override;
    double getsliderval(int seq);

private:
    Slider tauSlider;
    Slider pSlider;
    Slider dispersionSlider;
    Slider alpha1Slider;
    Slider alpha2Slider;
    Slider r1Slider;
    Slider r2Slider;
    Slider r3Slider;

    ImageComponent helpComp;

    Label tauLabel;
    Label pLabel;
    Label dispersionLabel;
    Label alpha1Label;
    Label alpha2Label;
    Label r1Label;
    Label r2Label;
    Label r3Label;

    StringModelAudioProcessor& processor;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> tauTree;
    //std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> omegaTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> pTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dispersionTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> alphaTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> alpha2Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> r1Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> r2Tree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> r3Tree;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> dimSelection;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Customize)
};


