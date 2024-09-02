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
    VisualPanel(FTMSynthAudioProcessor& p, Slider& attachedX, Slider& attachedY, Slider& attachedZ);
    ~VisualPanel() override;

    void setDimensions(int dim);
    void updateBounds();

    void paint(Graphics&) override;
    void resized() override;


    void updateXYonMouse(const MouseEvent& e);
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override;

private:
    FTMSynthAudioProcessor& processor;
    int dimensions;
    Slider& xSlider;
    Slider& ySlider;
    Slider& zSlider;

    Label thisIsALabel;
    Label nameLabel;

    Image strImage;
    Image delimiter;
    Image wire;
    Image drumSkin;

    Rectangle<float> bounds;
    float boundsDelta;
    bool mouseDownInBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualPanel)
};
