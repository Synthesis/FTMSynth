/*
  ==============================================================================

    VisualPanel.h
    Created: 25 Jun 2022 1:40:51pm
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
