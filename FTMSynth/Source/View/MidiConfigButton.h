/*
  ==============================================================================

    MidiConfigButton.h
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

#pragma once

#include <JuceHeader.h>

//==============================================================================
class MidiConfigButton : public ToggleButton
{
public:
    MidiConfigButton();
    ~MidiConfigButton() override;

    void setMapping(int cc, int channel);

protected:
    void paintButton(Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    Font standardFont;

private:
    int currentCC = -1;
    int currentChannel = -2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiConfigButton)
};
