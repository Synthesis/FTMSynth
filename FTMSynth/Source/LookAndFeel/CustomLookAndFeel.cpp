/*
  ==============================================================================

    CustomLookAndFeel.cpp
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

#include "CustomLookAndFeel.h"

//==============================================================================
CustomLookAndFeel::CustomLookAndFeel()
    : standardFont(Typeface::createSystemTypefaceFor(BinaryData::arial_narrow_7_ttf, BinaryData::arial_narrow_7_ttfSize))
{
    standardFont.setHeight(standardFont.getHeight() * 1.2f);
    standardFont.setHorizontalScale(1.0625f);
    standardFont.setExtraKerningFactor(1.0f/32.0f);

    setColour(ResizableWindow::backgroundColourId, Colour(0xFFD6C59A));

    setColour(TextButton::buttonColourId, Colour(0x00D6C59A));
    setColour(ToggleButton::textColourId, Colour(0xFF000000));
    setColour(ToggleButton::tickColourId, Colour(0xFF000000));
    setColour(DrawableButton::backgroundColourId, Colour(0x00D6C59A));
    setColour(DrawableButton::backgroundOnColourId, Colour(0x00D6C59A));

    setColour(Slider::backgroundColourId, Colour(0x00D6C59A));
    setColour(Slider::rotarySliderOutlineColourId, Colour(0xFF000000));
    setColour(Slider::textBoxTextColourId, Colour(0xFF000000));

    setColour(ComboBox::backgroundColourId, Colour(0xFFD6C59A));
    setColour(ComboBox::textColourId, Colour(0xFF000000));
    setColour(ComboBox::arrowColourId, Colour(0xFF000000));
    setColour(ComboBox::outlineColourId, Colour(0x80000000));
    setColour(PopupMenu::backgroundColourId, Colour(0xFF8F815B));
    setColour(PopupMenu::highlightedBackgroundColourId, Colour(0xFF50462B));

    setColour(BubbleComponent::backgroundColourId, Colour(0xFF8F815B));
    setColour(BubbleComponent::outlineColourId, Colour(0xFF8F815B));

    setColour(Label::textColourId, Colour(0xFF000000));

    setColour(TooltipWindow::backgroundColourId, Colour(0xFF8F815B));
    setColour(TooltipWindow::outlineColourId, Colour(0xFF8F815B));
}

Font CustomLookAndFeel::getTextButtonFont(TextButton&, int)
{
    return standardFont;
}

void CustomLookAndFeel::drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted,
                                         bool shouldDrawButtonAsDown)
{
    g.setFont(standardFont);
    LookAndFeel_V4::drawToggleButton(g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void CustomLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                         const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    float diameter = float(jmin(width, height));
    float centerX = float(x) + float(width) / 2.0f;
    float centerY = float(y) + float(height) / 2.0f;
    float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

    Image myKnob = ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);
    Rectangle<int> knobArea(0, 0, myKnob.getWidth()/3, myKnob.getHeight());
    Rectangle<int> indicatorArea(myKnob.getWidth()/3, 0, myKnob.getWidth()/3, myKnob.getHeight());
    Rectangle<int> shadowArea(myKnob.getWidth()*2/3, 0, myKnob.getWidth()/3, myKnob.getHeight());
    float imgCenterX = float(myKnob.getWidth()) / 6.0f;
    float imgCenterY = float(myKnob.getHeight()) / 2.0f;
    Image knob = myKnob.getClippedImage(knobArea);
    Image indicator = myKnob.getClippedImage(indicatorArea);
    Image indicatorExpanded(indicator.getFormat(), int(float(indicator.getWidth()) * 1.5f), int(float(indicator.getHeight()) * 1.5f), true);
    Graphics indicatorGraphics(indicatorExpanded);
    indicatorGraphics.fillAll(Colours::transparentBlack);
    indicatorGraphics.drawImage(indicator,
                                indicatorGraphics.getClipBounds().getCentreX() - indicator.getWidth()/2,
                                indicatorGraphics.getClipBounds().getCentreY() - indicator.getHeight()/2,
                                indicator.getWidth(),
                                indicator.getHeight(),
                                0, 0, indicator.getWidth(), indicator.getHeight());
    Image shadow = myKnob.getClippedImage(shadowArea);

    float scale = float(jmax(myKnob.getWidth()/3, myKnob.getHeight()));
    scale = diameter/scale;

    g.setImageResamplingQuality(Graphics::highResamplingQuality);

    // draw knob
    g.drawImageTransformed(knob, AffineTransform::
        translation(-imgCenterX, -imgCenterY)
        .scaled(scale, scale)
        .translated(centerX, centerY));
    // draw indicator
    NamedValueSet properties = slider.getProperties();
    if (properties.contains("colour"))
        g.setColour(Colours::white.overlaidWith(Colour(int(properties["colour"]))));
    g.drawImageTransformed(indicatorExpanded, AffineTransform::
        translation(-float(indicatorExpanded.getWidth())/2.0f, -float(indicatorExpanded.getHeight())/2.0f)
        .scaled(scale, scale)
        .rotated(angle)
        .translated(centerX, centerY),
        properties.contains("colour"));
    g.setColour(Colours::white);
    // draw shadow
    g.setOpacity(0.5f);
    g.drawImageTransformed(shadow, AffineTransform::
        translation(-imgCenterX, -imgCenterY)
        .scaled(scale, scale)
        .translated(centerX, centerY));
    g.setOpacity(1.0f);
}

int CustomLookAndFeel::getSliderPopupPlacement(Slider& slider)
{
    if (slider.getY() < 32)
        return BubbleComponent::below;
    else
        return BubbleComponent::above;
}

Font CustomLookAndFeel::getLabelFont(Label&)
{
    return standardFont;
}

BorderSize<int> CustomLookAndFeel::getLabelBorderSize(Label&)
{
    return BorderSize<int>(0, 0, 0, 0);
}


//==============================================================================
WithTextBox::WithTextBox()
{
    setColour(Label::backgroundColourId, Colour(0x10FFFFFF));
    setColour(Label::textColourId, Colour(0xFF000000));
    setColour(Label::outlineColourId, Colour(0x80FFFFFF));
    setColour(Label::backgroundWhenEditingColourId, Colours::transparentBlack);
    setColour(Label::textWhenEditingColourId, Colour(0xFFFFFFFF));
    setColour(Label::outlineWhenEditingColourId, Colours::transparentBlack);
}

void WithTextBox::drawLabel(Graphics& g, Label& label)
{
    g.fillAll(findColour(Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        float alpha = label.isEnabled() ? 1.0f : 0.5f;
        const Font font(getLabelFont(label));

        g.setColour(findColour(Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);

        Rectangle<int> textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         jmax(1, int(float(textArea.getHeight()) / font.getHeight())),
                         label.getMinimumHorizontalScale());

        g.setColour(findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour(findColour(Label::outlineColourId));
    }

    g.drawRoundedRectangle(label.getLocalBounds().toFloat(), 6.0f, 1.0f);
}


//==============================================================================
void DraggableBox::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float,
                                    const float, const float, Slider& slider)
{
    Rectangle<int> boxBounds(x, y, width, height);

    g.setFont(standardFont);
    g.setColour(slider.findColour(Slider::backgroundColourId).withAlpha(1.0f));
    g.fillRoundedRectangle(boxBounds.toFloat(), 6.0f);
    g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId).withAlpha(0.5f));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f), 6.0f, 1.0f);
    g.setColour(slider.findColour(Label::textColourId));
    g.drawFittedText(slider.getTextFromValue(slider.getValue()), x, y, width, height, Justification::centred, 1);
}


//==============================================================================
void CustomComboBox::drawComboBox(Graphics& g, int width, int height, bool, int, int, int, int,
                                    ComboBox& comboBox)
{
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(comboBox.findColour(ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), 6.0f);

    g.setColour(comboBox.findColour(ComboBox::outlineColourId).withAlpha(0.5f));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f), 6.0f, 1.0f);

    Rectangle<int> arrowZone(width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath(float(arrowZone.getX()) + 3.0f, float(arrowZone.getCentreY()) - 2.0f);
    path.lineTo(float(arrowZone.getCentreX()), float(arrowZone.getCentreY()) + 3.0f);
    path.lineTo(float(arrowZone.getRight()) - 3.0f, float(arrowZone.getCentreY()) - 2.0f);

    g.setColour(comboBox.findColour(ComboBox::arrowColourId).withAlpha(comboBox.isEnabled() ? 0.9f : 0.2f));
    g.strokePath(path, PathStrokeType(2.0f));
}

void CustomComboBox::drawLabel(Graphics& g, Label& label)
{
    float alpha = (label.isEnabled() ? 1.0f : 0.5f);
    const Font font(getLabelFont(label));

    g.setColour(findColour(ComboBox::textColourId).withMultipliedAlpha(alpha));
    g.setFont(font);

    Rectangle<int> textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds()).reduced(8, 0);

    g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                     jmax(1, int(float(textArea.getHeight()) / font.getHeight())),
                     label.getMinimumHorizontalScale());
}


//==============================================================================
FunnyFont::FunnyFont()
    : funnyFont(Typeface::createSystemTypefaceFor(BinaryData::_123Marker_ttf, BinaryData::_123Marker_ttfSize))
{
    funnyFont.setHeight(funnyFont.getHeight() * 2.0f);
}

Font FunnyFont::getLabelFont(Label&)
{
    return funnyFont;
}
