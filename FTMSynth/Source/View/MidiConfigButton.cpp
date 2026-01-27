/*
  ==============================================================================

    MidiConfigButton.cpp
    Created: 13 Oct 2024 10:01:23pm
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

#include <JuceHeader.h>
#include "MidiConfigButton.h"

//==============================================================================
MidiConfigButton::MidiConfigButton()
    : ToggleButton()
{
}

MidiConfigButton::~MidiConfigButton()
{
}

void MidiConfigButton::setMapping(int cc, int channel)
{
    currentCC = cc;
    currentChannel = channel;
    repaint();
}
/*
void MidiConfigButton::paint(juce::Graphics& g)
{
}

void MidiConfigButton::resized()
{
}
*/

void MidiConfigButton::paintButton(Graphics &g,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    Rectangle<float> bounds = getBounds().toFloat();
    bounds.setPosition(0.0f, 0.0f);

    g.setColour(Colour(0x3F000000));
    if (shouldDrawButtonAsHighlighted) g.setColour(Colour(0x3FFFFFFF));
    if (shouldDrawButtonAsDown || getToggleState()) g.setColour(Colour(0xA7FFFFFF));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f, 0.5f), 6.0f, 1.0f);

    if (shouldDrawButtonAsDown || getToggleState())
    {
        g.setColour(Colour(0xFFDFCEA1));
        g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f, 1.0f), 5.0f);
    }

    String strCC = "CC  ";
    if (currentCC == -1) strCC += "---";
    else                 strCC += String(currentCC);

    String strChannel = "CH  ";
    if (currentChannel == -2)      strChannel += "--";
    else if (currentChannel == -1) strChannel += "OM";
    else                           strChannel += String(currentChannel + 1);

    g.setColour(Colour(0xFF000000));
    g.drawText(strCC, bounds, Justification::centredTop);
    g.setColour(Colour(0x7F000000));
    g.drawText(strChannel, bounds, Justification::centredBottom);
}
