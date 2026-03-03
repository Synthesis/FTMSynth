/*
  ==============================================================================

    CustomDrawableButton.h
    Created: 16 Feb 2026 12:15:59am
    Author:  Loïc J

  ==============================================================================

    This file is part of FTMSynth.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class CustomDrawableButton  : public DrawableButton
{
public:
    enum Style { Default, Borderless };

    CustomDrawableButton(const String &buttonName);
    CustomDrawableButton(const String &buttonName, Style style);

    void setStyle(Style newStyle);

    Rectangle<float> getImageBounds() const override;

    void paintButton(Graphics &g,
                     bool shouldDrawButtonAsHighlighted,
                     bool shouldDrawButtonAsDown) override;

private:
    Style sStyle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomDrawableButton)
};
