/*
  ==============================================================================

    MainView.h
    Created: 4 Oct 2024 12:03:18am
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
#include "VisualPanel.h"

//==============================================================================
class MainView : public juce::Component
{
public:
    MainView(FTMSynthAudioProcessor& p);
    ~MainView() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setDimensions(int dimensions, bool btnToSlider);
    void updateDimensionComponents();
    void updateVisualization(bool updateMouseBounds = false);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FTMSynthAudioProcessor& processor;

    // Custom look-and-feel
    WithTextBox withTextBox;
    DraggableBox draggableBox;
    CustomComboBox customComboBox;
    FunnyFont funnyFont;

    // Help tooltip
    SharedResourcePointer<TooltipWindow> tooltip;

    // UI components
    Slider dimensionsSlider;
    ImageButton stringButton;
    ImageButton drumButton;
    ImageButton boxButton;

    Slider volumeSlider;
    Slider attackSlider;
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

    Label thisIsALabel;
    Label nameLabel;
    VisualPanel visualPanel;

    // Attachments from model to components
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dimTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volumeTree;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackTree;
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainView)
};
