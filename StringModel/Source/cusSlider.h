/*
  ==============================================================================

    cusSlider.h
    Created: 7 Mar 2020 9:24:44pm
    Author:  Lily H

  ==============================================================================
*/

#pragma once
class cusSlider : public LookAndFeel_V4
{
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        float diameter = jmin(width-8,height-8); //compare two values, get min value
        float radius = diameter/2;
        float centreX = x + width/2; // get extact centre x
        float centreY =  y + height/2; // get extact centre y
        float rx = centreX - radius;
        float ry = centreY - radius;
        float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));
        float pointerThickness = 4.0;

        //std::cout << rotaryStartAngle << " " << rotaryEndAngle << std::endl;
        Rectangle<float> dialArea (rx, ry, diameter, diameter);

        /* Draw Background */
        g.setColour(Colour::fromFloatRGBA(0.84,0.8,0.73,0.9));
        g.fillEllipse(dialArea);

        /* Draw Center */
        //g.setColour(Colours::black);
        //g.fillEllipse(centreX - (pointerThickness/1.5), centreY - (pointerThickness/1.5), pointerThickness*1.5,pointerThickness*1.5);

        Path dialTick;
        dialTick.addRoundedRectangle(-pointerThickness/2, -radius, pointerThickness, radius, radius/9);

        g.setColour(Colours::black);
        g.fillPath(dialTick, AffineTransform::rotation(angle).translated(centreX, centreY));

        /* Draw Outline*/
        g.setColour(Colours::black);
        g.setOpacity(0.6);
        g.drawEllipse(rx, ry, diameter, diameter, 4.0f/*Pixl*/);
    }
};
