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

#include "MidiConfigButton.h"

//==============================================================================
MidiConfigButton::MidiConfigButton()
    : ToggleButton(),
      standardFont(FontOptions(Typeface::createSystemTypefaceFor(BinaryData::arial_narrow_7_ttf, BinaryData::arial_narrow_7_ttfSize)))
{
    standardFont.setPointHeight(15.0f);
    standardFont.setAscentOverride(0.9f);
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

void MidiConfigButton::paintButton(Graphics &g,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    Rectangle<float> bounds = getLocalBounds().toFloat();
    bounds.setPosition(0.f, 0.f);
    float width  = bounds.getWidth();
    float height = bounds.getHeight();

    bool flatLeft   = isConnectedOnLeft();
    bool flatRight  = isConnectedOnRight();
    bool flatTop    = isConnectedOnTop();
    bool flatBottom = isConnectedOnBottom();

    g.setColour(Colour(0x3F000000));
    if (shouldDrawButtonAsHighlighted) g.setColour(Colour(0x3FFFFFFF));
    if (shouldDrawButtonAsDown || getToggleState()) g.setColour(Colour(0xA7FFFFFF));

    if (flatLeft || flatRight || flatTop || flatBottom)
    {
        Path stroke;
        stroke.addRoundedRectangle(0.5f, 0.5f, width - 1.f, height - 1.f,
                                   6.0f, 6.0f,  // corner size
                                   !(flatLeft  || flatTop),
                                   !(flatRight || flatTop),
                                   !(flatLeft  || flatBottom),
                                   !(flatRight || flatBottom));
        g.strokePath(stroke, PathStrokeType(1.0f));
    }
    else
    {
        g.drawRoundedRectangle(bounds.reduced(0.5f, 0.5f), 6.0f, 1.0f);
    }

    if (shouldDrawButtonAsDown || getToggleState())
    {
        g.setColour(Colour(0xFFDFCEA1));
    }
    else
    {
        g.setColour(Colour(0xFFD6C59A));
    }
    if (flatLeft || flatRight || flatTop || flatBottom)
    {
        Path fill;
        fill.addRoundedRectangle(1.f, 1.f, width - 2.f, height - 2.f,
                                5.0f, 5.0f,  // corner size
                                !(flatLeft  || flatTop),
                                !(flatRight || flatTop),
                                !(flatLeft  || flatBottom),
                                !(flatRight || flatBottom));
        g.fillPath(fill);
    }
    else
    {
        g.fillRoundedRectangle(bounds.reduced(1.0f, 1.0f), 5.0f);
    }

    String strCC = "CC ";
    if (currentCC == -1) strCC += "-";
    else                 strCC += String(currentCC);

    String strChannel = "CH ";
    if (currentChannel == -2)      strChannel += "-";
    else if (currentChannel == -1) strChannel += "OM";
    else                           strChannel += String(currentChannel + 1);

    // Vertical layout (default)
    Rectangle<float> ccTextRect(0.f, 0.f, width, height / 2.f);
    Rectangle<float> channelTextRect(0.f, height / 2.f, width, height / 2.f);
    if (height < 32.f && width >= 64.f)
    {
        // Horizontal layout
        ccTextRect.setSize(width / 2.f, height);
        channelTextRect.setBounds(width / 2.f, 0.f, width / 2.f, height);
    }

    g.setFont(standardFont);

    g.setColour(Colour(0x9F000000));
    g.drawText(strCC, ccTextRect, Justification::centred);
    g.setColour(Colour(0x6F000000));
    g.drawText(strChannel, channelTextRect, Justification::centred);
}
