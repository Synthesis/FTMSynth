/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 20 Jun 2022 2:14:06pm
    Author:  Loïc J

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    CustomLookAndFeel()
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

    Font getTextButtonFont(TextButton&, int buttonHeight) override
    {
        return standardFont;
    }

    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        g.setFont(standardFont);
        LookAndFeel_V4::drawToggleButton(g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        float diameter = jmin(width, height);
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

        Image myKnob = ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);
        Rectangle<int> knobArea(0, 0, myKnob.getWidth()/3, myKnob.getHeight());
        Rectangle<int> indicatorArea(myKnob.getWidth()/3, 0, myKnob.getWidth()/3, myKnob.getHeight());
        Rectangle<int> shadowArea(myKnob.getWidth()*2/3, 0, myKnob.getWidth()/3, myKnob.getHeight());
        float imgCenterX = myKnob.getWidth() / 6;
        float imgCenterY = myKnob.getHeight() / 2;
        Image knob = myKnob.getClippedImage(knobArea);
        Image indicator = myKnob.getClippedImage(indicatorArea);
        Image shadow = myKnob.getClippedImage(shadowArea);

        float scale = jmax(myKnob.getWidth()/3, myKnob.getHeight());
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
        g.drawImageTransformed(indicator, AffineTransform::
            translation(-imgCenterX, -imgCenterY)
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

    int getSliderPopupPlacement(Slider& slider) override
    {
        if (slider.getY() < 32)
            return BubbleComponent::below;
        else
            return BubbleComponent::above;
    }

    Font getLabelFont(Label&) override
    {
        return standardFont;
    }

    BorderSize<int> getLabelBorderSize(Label&) override
    {
        return BorderSize<int>(0, 0, 0, 0);
    }


protected:
    Font standardFont;
};


class WithTextBox : public CustomLookAndFeel
{
public:
    WithTextBox()
    {
        setColour(Label::backgroundColourId, Colour(0x10FFFFFF));
        setColour(Label::textColourId, Colour(0xFF000000));
        setColour(Label::outlineColourId, Colour(0x80FFFFFF));
        setColour(Label::backgroundWhenEditingColourId, Colours::transparentBlack);
        setColour(Label::textWhenEditingColourId, Colour(0xFFFFFFFF));
        setColour(Label::outlineWhenEditingColourId, Colours::transparentBlack);
    }

    void drawLabel(Graphics& g, Label& label) override
    {
        g.fillAll(findColour(Label::backgroundColourId));

        if (!label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font(getLabelFont(label));

            g.setColour(findColour(Label::textColourId).withMultipliedAlpha(alpha));
            g.setFont(font);

            auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

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
};


class DraggableBox : public CustomLookAndFeel
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        Rectangle<int> boxBounds(x, y, width, height);

        g.setFont(standardFont);
        g.setColour(slider.findColour(Slider::backgroundColourId).withAlpha(1.0f));
        g.fillRoundedRectangle(boxBounds.toFloat(), 6.0f);
        g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId).withAlpha(0.5f));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), 6.0f, 1.0f);
        g.setColour(slider.findColour(Label::textColourId));
        g.drawFittedText(String(int(slider.getValue())), x, y, width, height, Justification::centred, 1);
    }
};


class CustomComboBox : public CustomLookAndFeel
{
public:
    void drawComboBox(Graphics& g, int width, int height, bool isButtonDown, int, int, int, int,
        ComboBox& comboBox) override
    {
        Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(comboBox.findColour(ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds.toFloat(), 6.0f);

        g.setColour(comboBox.findColour(ComboBox::outlineColourId).withAlpha(0.5f));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), 6.0f, 1.0f);

        Rectangle<int> arrowZone(width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath(float(arrowZone.getX()) + 3.0f, float(arrowZone.getCentreY()) - 2.0f);
        path.lineTo(float(arrowZone.getCentreX()), float(arrowZone.getCentreY()) + 3.0f);
        path.lineTo(float(arrowZone.getRight()) - 3.0f, float(arrowZone.getCentreY()) - 2.0f);

        g.setColour(comboBox.findColour(ComboBox::arrowColourId).withAlpha(comboBox.isEnabled() ? 0.9f : 0.2f));
        g.strokePath(path, PathStrokeType(2.0f));
    }

    void drawLabel(Graphics& g, Label& label) override
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
};


class FunnyFont : public CustomLookAndFeel
{
public:
    FunnyFont()
    : funnyFont(Typeface::createSystemTypefaceFor(BinaryData::_123Marker_ttf, BinaryData::_123Marker_ttfSize))
    {
        funnyFont.setHeight(funnyFont.getHeight() * 2.0f);
    }

    Font getLabelFont(Label&) override
    {
        return funnyFont;
    }

private:
    Font funnyFont;
};
