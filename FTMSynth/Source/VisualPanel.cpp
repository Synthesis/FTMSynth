/*
  ==============================================================================

    VisualPanel.cpp
    Created: 25 Jun 2022 1:40:51pm
    Author:  Loïc J

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VisualPanel.h"
#include "CustomLookAndFeel.h"

//==============================================================================
VisualPanel::VisualPanel(FTMSynthAudioProcessor& p, int dim)
    : processor(p), dimensions(dim)
{
    setLookAndFeel(new FunnyFont());

    String typeStr("this is\na");

    thisIsALabel.setText(typeStr, dontSendNotification);
    thisIsALabel.setJustificationType(Justification(Justification::topLeft));
    thisIsALabel.setColour(Label::textColourId, Colour(0xFF5F5F5F));
    addAndMakeVisible(thisIsALabel);

    String objStr("");
    if (dimensions == 1) objStr += "string.";
    else if (dimensions == 2) objStr += "drum.";
    else if (dimensions == 3) objStr += "cuboid.";

    nameLabel.setText(objStr, dontSendNotification);
    nameLabel.setJustificationType(Justification(Justification::topLeft));
    addAndMakeVisible(nameLabel);
}

VisualPanel::~VisualPanel()
{
}

void VisualPanel::paint(juce::Graphics& g)
{
    // g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    // g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 4);
    // g.setColour(Colours::black);

    if (dimensions == 1)
    {
        float r1 = processor.tree.getRawParameterValue("r1")->load();

        Image strImage = ImageCache::getFromMemory(BinaryData::string_png, BinaryData::string_pngSize);
        Image delimiter = strImage.getClippedImage(Rectangle<int>(0, 0, strImage.getWidth()/2, strImage.getHeight()));
        Image wire = strImage.getClippedImage(Rectangle<int>(strImage.getWidth()/2, 0, strImage.getWidth()/2, strImage.getHeight()));

        int left = 120;
        int right = 312;
        int y = 112;

        // draw string
        for (int i = left; i < right; i += 16)
            g.drawImageAt(wire, i+8, y);

        float length = float(right-left)-12.0f;
        float posX = float(left)+14.0f+r1*length;
        float posY = float(y)+2;
        float triangleW = 5.0f;
        float triangleH = 10.0f;

        // draw position
        Path position;
        position.addTriangle(posX-triangleW, posY-triangleH, posX+triangleW, posY-triangleH, posX, posY);
        g.setColour(Colours::red);
        g.fillPath(position);
        g.setColour(Colours::black);

        // draw ends
        g.drawImageAt(delimiter, left, y);
        g.drawImageAt(delimiter, right, y);
    }
    else if (dimensions == 2)
    {
        float alpha1 = processor.tree.getRawParameterValue("squareness")->load();
        float r1 = processor.tree.getRawParameterValue("r1")->load();
        float r2 = processor.tree.getRawParameterValue("r2")->load();
        r2 = 1-r2; // Y is inverted on a screen

        float thickness = 3.0f;

        float destWidth = 192.0f;
        float destHeight = 192.0f*alpha1;
        float destX = 128.0f;
        float destY = 108.0f-(destHeight/2.0f);

        // draw drum skin
        Image drumSkin = ImageCache::getFromMemory(BinaryData::drum_skin_png, BinaryData::drum_skin_pngSize);
        float srcX = 0.f;
        float srcY = (1-alpha1)*drumSkin.getHeight()/2.0f;
        float srcWidth = drumSkin.getWidth();
        float srcHeight = alpha1*drumSkin.getHeight();
        g.drawImage(drumSkin, destX, destY, destWidth, destHeight, srcX, srcY, srcWidth, srcHeight);

        // draw red cross at impact position
        g.setColour(Colours::red);
        float pointThickness = 3.0f;
        float crossSize = 5.0f;
        g.drawLine(destX+(r1*destWidth)-crossSize, destY+(r2*destHeight),
                   destX+(r1*destWidth)+crossSize, destY+(r2*destHeight), pointThickness);
        g.drawLine(destX+(r1*destWidth), destY+(r2*destHeight)-crossSize,
                   destX+(r1*destWidth), destY+(r2*destHeight)+crossSize, pointThickness);

        // draw bounds
        g.setColour(Colour(0xFF451A08));
        g.drawRect(destX-(thickness/2.0f), destY-(thickness/2.0f), destWidth+thickness, destHeight+thickness, thickness);
        g.setColour(Colours::black);
    }
    else if (dimensions == 3)
    {
        float alpha1 = processor.tree.getRawParameterValue("squareness")->load();
        float alpha2 = processor.tree.getRawParameterValue("cubeness")->load();
        float r1 = processor.tree.getRawParameterValue("r1")->load();
        float r2 = processor.tree.getRawParameterValue("r2")->load();
        float r3 = processor.tree.getRawParameterValue("r3")->load();
        r2 = 1-r2; // vertical direction is inverted on a screen
        r3 = 1-r3;

        float thickness = 3.0f;

        float width = 128.0f;
        float depth = 128.0f*alpha2;
        float height = 64.0f*alpha1;

        // compute coordinates of cube vertices
        float frontLeft = 160.0f-(height/2.0f);
        float frontTop = 108.0f-(depth/2.0f)+(height/2.0f);
        float frontRight = frontLeft + width;
        float frontBottom = frontTop + depth;
        float backLeft = 160.0f+(height/2.0f);
        float backTop = 108.0f-(depth/2.0f)-(height/2.0f);
        float backRight = backLeft + width;
        float backBottom = backTop + depth;

        // draw backface and edge
        g.setColour(Colour(0xFF7F7F7F));
        g.drawRect(backLeft-(thickness/2.0f), backTop-(thickness/2.0f), width+thickness, depth+thickness, thickness);
        g.drawLine(frontLeft, frontBottom, backLeft, backBottom, thickness);

        // draw red cross at impact position
        g.setColour(Colours::red);
        float pointThickness = 3.0f;
        float crossSize = 5.0f;
        g.drawLine(backLeft+(r1*width)-(r2*height)-crossSize, backTop+(r2*height)+(r3*depth),
                   backLeft+(r1*width)-(r2*height)+crossSize, backTop+(r2*height)+(r3*depth), pointThickness);
        g.drawLine(backLeft+(r1*width)-(r2*height), backTop+(r2*height)+(r3*depth)-crossSize,
                   backLeft+(r1*width)-(r2*height), backTop+(r2*height)+(r3*depth)+crossSize, pointThickness);

        // define top and right faces
        Path parallelogram1;
        parallelogram1.addQuadrilateral(frontLeft, frontTop, backLeft, backTop,
                                        backRight, backTop, frontRight, frontTop);
        Path parallelogram2;
        parallelogram2.addQuadrilateral(frontRight, frontTop, backRight, backTop,
                                        backRight, backBottom, frontRight, frontBottom);

        // fill top, right and front faces with translucent white
        g.setColour(Colour(0x5FFFFFFF));
        g.fillPath(parallelogram1);
        g.fillPath(parallelogram2);
        g.fillRect(frontLeft, frontTop, width, depth);

        // draw cube contour (front face + other outer edges)
        g.setColour(Colours::black);
        g.drawLine(frontLeft, frontTop, backLeft, backTop, thickness);
        g.drawLine(backLeft, backTop, backRight, backTop, thickness);
        g.drawLine(frontRight, frontTop, backRight, backTop, thickness);
        g.drawLine(backRight, backTop, backRight, backBottom, thickness);
        g.drawLine(backRight, backBottom, frontRight, frontBottom, thickness);
        g.drawRect(frontLeft-(thickness/2.0f), frontTop-(thickness/2.0f), width+thickness, depth+thickness, thickness);
    }
}

void VisualPanel::resized()
{
    int offsetX = 16;
    int offsetY = 12;

    thisIsALabel.setBounds(offsetX, offsetY, 256, 96);
    nameLabel.setBounds(offsetX + 20, offsetY + 28, 240, 48);
}
