/*
  ==============================================================================

    HelpPanel.cpp
    Created: 4 Feb 2026 11:25:12pm
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
#include "HelpPanel.h"
#include "juce_core/juce_core.h"

//==============================================================================
HelpPanel::HelpPanel()
{
}

HelpPanel::~HelpPanel()
{
}

void HelpPanel::paint(juce::Graphics& g)
{
    g.setColour(Colour(0x9F000000));
    g.drawMultiLineText(CharPointer_UTF8(
            "This synth uses physical modeling to create percussive sounds. "
            "Choose from three models: string, rectangular drum and cuboid box. "
            "Use the knobs to sculpt unique timbral characters.\n\n"
            "(Hover mouse over knob labels for details)"),
        getLocalBounds().getX(),
        getLocalBounds().getY() + 24,
        getLocalBounds().getWidth(),
        Justification::centred);

    g.drawText(CharPointer_UTF8("\xe2\x80\x94 Credits \xe2\x80\x94"),
               getLocalBounds().getX(),
               getLocalBounds().getBottom() - 64,
               getLocalBounds().getWidth(), 14,
               Justification::centred);
    g.drawText(CharPointer_UTF8("engine"),
               getLocalBounds().getCentreX() + 4,
               getLocalBounds().getBottom() - 44,
               getLocalBounds().getWidth()/2 - 4, 14,
               Justification::left);
    g.drawText(CharPointer_UTF8("optimization, UI"),
               getLocalBounds().getCentreX() + 4,
               getLocalBounds().getBottom() - 28,
               getLocalBounds().getWidth()/2 - 4, 14,
               Justification::left);

    g.setColour(Colour(0xFF000000));
    g.drawText(CharPointer_UTF8("Han Han"),
               getLocalBounds().getX(),
               getLocalBounds().getBottom() - 44,
               getLocalBounds().getWidth()/2 - 4, 14,
               Justification::right);
    g.drawText(CharPointer_UTF8("Lo\xc3\xaf""c Jankowiak"),
               getLocalBounds().getX(),
               getLocalBounds().getBottom() - 28,
               getLocalBounds().getWidth()/2 - 4, 14,
               Justification::right);
}

void HelpPanel::resized()
{
}
