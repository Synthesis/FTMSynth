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

        setColour(ResizableWindow::backgroundColourId, Colour(0xFFCBBB92));

        setColour(TextButton::buttonColourId, Colour(0x00CBBB92));
        setColour(ToggleButton::textColourId, Colour(0xFF000000));
        setColour(ToggleButton::tickColourId, Colour(0xFF000000));
        setColour(DrawableButton::backgroundColourId, Colour(0x00CBBB92));
        setColour(DrawableButton::backgroundOnColourId, Colour(0x00CBBB92));
        setColour(ComboBox::outlineColourId, Colour(0x00000000));

        setColour(Slider::backgroundColourId, Colour(0x00CBBB92));
        setColour(Slider::textBoxTextColourId, Colour(0xFF000000));
        setColour(Slider::textBoxOutlineColourId, Colour(0xFF000000));

        setColour(BubbleComponent::backgroundColourId, Colour(0xFF8F815B));
        setColour(BubbleComponent::outlineColourId, Colour(0xFF8F815B));

        setColour(Label::textColourId, Colour(0xFF000000));

        setColour(TooltipWindow::backgroundColourId, Colour(0xFF8F815B));
        setColour(TooltipWindow::outlineColourId, Colour(0xFF8F815B));
    }

    Font getTextButtonFont (TextButton&, int buttonHeight) override
    {
        return standardFont;
    }

    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        g.setFont(standardFont);
        LookAndFeel_V4::drawToggleButton(g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        // g.drawFittedText(button.getButtonText(), 0, 0, button.getWidth(), button.getHeight(),
        //                  Justification::centred, 1);
    }

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        float diameter = jmin(width, height);
        float radius = diameter / 2;
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

        Image myKnob = ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);
        Rectangle<int> knobArea (0, 0, myKnob.getWidth()/3, myKnob.getHeight());
        Rectangle<int> indicatorArea (myKnob.getWidth()/3, 0, myKnob.getWidth()/3, myKnob.getHeight());
        Rectangle<int> shadowArea (myKnob.getWidth()*2/3, 0, myKnob.getWidth()/3, myKnob.getHeight());
        Image knob = myKnob.getClippedImage(knobArea);
        Image indicator = myKnob.getClippedImage(indicatorArea);
        Image shadow = myKnob.getClippedImage(shadowArea);

        float scale = jmax(myKnob.getWidth()/3, myKnob.getHeight());
        scale = diameter/scale;

        // g.setImageResamplingQuality(Graphics::highResamplingQuality);

        // draw knob
        g.drawImageTransformed(knob, AffineTransform::scale(scale, scale));
        // draw indicator
        g.drawImageTransformed(indicator, AffineTransform::
            scale(scale, scale)
            .translated(-centerX, -centerY)
            .rotated(angle)
            .translated(centerX, centerY));
        // draw shadow
        g.setOpacity(0.75f);
        g.drawImageTransformed(shadow, AffineTransform::scale(scale, scale));
        g.setOpacity(1.0f);
    }

    int getSliderPopupPlacement(Slider& slider) override
    {
        return 0;
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


class DraggableBox : public CustomLookAndFeel
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        g.setFont(standardFont);
        g.setColour(findColour(Slider::backgroundColourId).withAlpha(1.0f));
        g.fillRoundedRectangle(x, y, width, height, 6);
        g.setColour(Colours::black);
        g.drawRoundedRectangle(x, y, width, height, 6, 1);
        g.drawFittedText(String(int(slider.getValue())), x, y, width, height, Justification::centred, 1);
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