/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 20 Jun 2022 2:14:06pm
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
const Rectangle<int> mainControls(112, 8, 512, 158);
const Rectangle<int> xyzControls(416, 176, 208, 216);
const float alphaOff = 0.125f;

//==============================================================================
class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    CustomLookAndFeel();

    Font getTextButtonFont(TextButton&, int) override;

    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;

    int getSliderPopupPlacement(Slider& slider) override;

    Font getLabelFont(Label&) override;
    BorderSize<int> getLabelBorderSize(Label&) override;

    void drawPopupMenuBackground(Graphics& g, int width, int height) override;
    void drawPopupMenuBackgroundWithOptions(Graphics& g,
                                            int width,
                                            int height,
                                            const PopupMenu::Options&) override;

protected:
    Font standardFont;
};


class WithTextBox : public CustomLookAndFeel
{
public:
    WithTextBox();

    void drawLabel(Graphics& g, Label& label) override;
};


class DraggableBox : public CustomLookAndFeel
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float,
                          const float, const float, Slider& slider) override;
};


class CustomComboBox : public CustomLookAndFeel
{
public:
    void drawComboBox(Graphics& g, int width, int height, bool, int, int, int, int,
        ComboBox& comboBox) override;

    void drawLabel(Graphics& g, Label& label) override;
};


class FunnyFont : public CustomLookAndFeel
{
public:
    FunnyFont();

    Font getLabelFont(Label&) override;

private:
    Font funnyFont;
};
