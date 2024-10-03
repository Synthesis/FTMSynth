/*
  ==============================================================================

    VisualPanel.cpp
    Created: 25 Jun 2022 1:40:51pm
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
#include "VisualPanel.h"
#include "CustomLookAndFeel.h"

//==============================================================================
VisualPanel::VisualPanel(AudioProcessorValueTreeState& treeState, Slider& attachedX, Slider& attachedY, Slider& attachedZ)
    : tree(treeState), xSlider(attachedX), ySlider(attachedY), zSlider(attachedZ),
      boundsDelta(12.0f), mouseDownInBounds(false)
{
    setLookAndFeel(new FunnyFont());

    String typeStr("this is\na");

    thisIsALabel.setText(typeStr, dontSendNotification);
    thisIsALabel.setJustificationType(Justification(Justification::topLeft));
    thisIsALabel.setColour(Label::textColourId, Colour(0xFF5F5F5F));
    thisIsALabel.setInterceptsMouseClicks(false, true);
    addChildComponent(thisIsALabel);
    nameLabel.setInterceptsMouseClicks(false, true);
    addChildComponent(nameLabel);

    // 1D resources
    strImage = ImageCache::getFromMemory(BinaryData::string_png, BinaryData::string_pngSize);
    delimiter = strImage.getClippedImage(Rectangle<int>(0, 0, strImage.getWidth()/2, strImage.getHeight()));
    wire = strImage.getClippedImage(Rectangle<int>(strImage.getWidth()/2, 0, strImage.getWidth()/2, strImage.getHeight()));

    // 2D resources
    drumSkin = ImageCache::getFromMemory(BinaryData::drum_skin_png, BinaryData::drum_skin_pngSize);
}

VisualPanel::~VisualPanel()
{
}

void VisualPanel::setDimensions(int dim)
{
    dimensions = dim;
    updateBounds();
}

void VisualPanel::updateBounds()
{
    // TODO set const coordinate offset (x, y) for clickable visualization
    // (in order to avoid manually changing every variable when repositioning the widget)

    if (dimensions == 1)
    {
        bounds.setBounds(152.0f, 104.0f, 192.0f, 32.0f);
    }
    if (dimensions == 2)
    {
        float alpha1 = tree.getRawParameterValue("squareness")->load();
        bounds.setBounds(152.0f, 108.0f-(192.0f*alpha1/2.0f),
                         192.0f, 192.0f*alpha1);
    }
    if (dimensions == 3)
    {
        float alpha1 = tree.getRawParameterValue("squareness")->load();
        float alpha2 = tree.getRawParameterValue("cubeness")->load();
        float r3 = tree.getRawParameterValue("r3")->load();
        float height = 128.0f*alpha1;
        float depth = 64.0f*alpha2;
        bounds.setBounds(184.0f+((r3-0.5f)*depth), 108.0f-(height*0.5f)+((0.5f-r3)*depth),
                         128.0f, height);
    }
}

void VisualPanel::paint(Graphics& g)
{
    String objStr("");

    if (dimensions == 1)
    {
        objStr += "string.";

        float r1 = tree.getRawParameterValue("r1")->load();

        int left = 144;
        int right = 336;
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
        objStr += "drum.";

        float alpha1 = tree.getRawParameterValue("squareness")->load();
        float r1 = tree.getRawParameterValue("r1")->load();
        float r2 = tree.getRawParameterValue("r2")->load();
        r2 = 1-r2; // Y is inverted on a screen

        float thickness = 3.0f;

        float destWidth = 192.0f;
        float destHeight = 192.0f*alpha1;
        float destX = 152.0f;
        float destY = 108.0f-(destHeight/2.0f);

        float impulseX = destX+(r1*destWidth);
        float impulseY = destY+(r2*destHeight);

        // draw drum skin
        float srcX = 0.f;
        float srcY = (1-alpha1)*drumSkin.getHeight()/2.0f;
        float srcWidth = float(drumSkin.getWidth());
        float srcHeight = alpha1*drumSkin.getHeight();
        g.drawImage(drumSkin, int(destX), int(destY), int(destWidth), int(destHeight),
                    int(srcX), int(srcY), int(srcWidth), int(srcHeight));

        // draw red cross at impact position
        g.setColour(Colours::red);
        float pointThickness = 3.0f;
        float crossSize = 5.0f;
        g.drawLine(impulseX-crossSize, impulseY, impulseX+crossSize, impulseY, pointThickness);
        g.drawLine(impulseX, impulseY-crossSize, impulseX, impulseY+crossSize, pointThickness);

        // draw bounds
        g.setColour(Colour(0xFF451A08));
        g.drawRect(destX-(thickness/2.0f), destY-(thickness/2.0f), destWidth+thickness, destHeight+thickness, thickness);
        g.setColour(Colours::black);
    }
    else if (dimensions == 3)
    {
        objStr += "cuboid.";

        float alpha1 = tree.getRawParameterValue("squareness")->load();
        float alpha2 = tree.getRawParameterValue("cubeness")->load();
        float r1 = tree.getRawParameterValue("r1")->load();
        float r2 = tree.getRawParameterValue("r2")->load();
        float r3 = tree.getRawParameterValue("r3")->load();
        r2 = 1-r2; // vertical direction is inverted on a screen
        r3 = 1-r3;

        float thickness = 3.0f;
        float zPlaneThickness = 1.0f;
        Colour zPlaneColour(0x0FFF0000);
        Colour zPlaneContourColour(0x4FFF0000);

        float width = 128.0f;
        float height = 128.0f*alpha1;
        float depth = 64.0f*alpha2;

        // compute coordinates of cube vertices
        float frontLeft = 184.0f-(depth/2.0f);
        float frontTop = 108.0f-(height/2.0f)+(depth/2.0f);
        float frontRight = frontLeft + width;
        float frontBottom = frontTop + height;
        float backLeft = 184.0f+(depth/2.0f);
        float backTop = 108.0f-(height/2.0f)-(depth/2.0f);
        float backRight = backLeft + width;
        float backBottom = backTop + height;

        float impulseX = backLeft+(r1*width)-(r3*depth);
        float impulseY = backTop+(r3*depth)+(r2*height);

        // draw backface and edge
        g.setColour(Colour(0xFF7F7F7F));
        g.drawRect(backLeft-(thickness/2.0f), backTop-(thickness/2.0f), width+thickness, height+thickness, thickness);
        g.drawLine(frontLeft, frontBottom, backLeft, backBottom, thickness);

        // draw left and bottom impact depth-plane contour
        g.setColour(zPlaneContourColour);
        g.drawLine(backLeft-(r3*depth), backTop+(r3*depth),
                   backLeft-(r3*depth), backTop+(r3*depth)+height, zPlaneThickness);
        g.drawLine(backLeft-(r3*depth), backTop+(r3*depth)+height,
                   backLeft+width-(r3*depth), backTop+(r3*depth)+height, zPlaneThickness);

        // draw red cross at impact position
        g.setColour(Colours::red);
        float pointThickness = 3.0f;
        float crossSize = 5.0f;
        g.drawLine(impulseX-crossSize, impulseY, impulseX+crossSize, impulseY, pointThickness);
        g.drawLine(impulseX, impulseY-crossSize, impulseX, impulseY+crossSize, pointThickness);

        // draw depth plane in translucent red
        g.setColour(zPlaneColour);
        g.fillRect(backLeft-(r3*depth), backTop+(r3*depth), width, height);

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
        g.fillRect(frontLeft, frontTop, width, height);

        // draw top and right impact depth-plane contour
        g.setColour(zPlaneContourColour);
        g.drawLine(backLeft-(r3*depth), backTop+(r3*depth),
                   backLeft+width-(r3*depth), backTop+(r3*depth), zPlaneThickness);
        g.drawLine(backLeft+width-(r3*depth), backTop+(r3*depth),
                   backLeft+width-(r3*depth), backTop+(r3*depth)+height, zPlaneThickness);

        // draw cube contour (front face + other outer edges)
        g.setColour(Colours::black);
        g.drawLine(frontLeft, frontTop, backLeft, backTop, thickness);
        g.drawLine(backLeft, backTop, backRight, backTop, thickness);
        g.drawLine(frontRight, frontTop, backRight, backTop, thickness);
        g.drawLine(backRight, backTop, backRight, backBottom, thickness);
        g.drawLine(backRight, backBottom, frontRight, frontBottom, thickness);
        g.drawRect(frontLeft-(thickness/2.0f), frontTop-(thickness/2.0f), width+thickness, height+thickness, thickness);
    }

    nameLabel.setText(objStr, dontSendNotification);
    nameLabel.setJustificationType(Justification(Justification::topLeft));

    if (dimensions >= 1 && dimensions <= 3)
    {
        if (! thisIsALabel.isVisible()) thisIsALabel.setVisible(true);
        if (! nameLabel.isVisible()) nameLabel.setVisible(true);
    }
    else
    {
        if (thisIsALabel.isVisible()) thisIsALabel.setVisible(false);
        if (nameLabel.isVisible()) nameLabel.setVisible(false);
    }
}

void VisualPanel::resized()
{
    int offsetX = 16;
    int offsetY = 98;

    thisIsALabel.setBounds(offsetX, offsetY, 96, 64);
    nameLabel.setBounds(offsetX + 20, offsetY + 28, 80, 40);
}

void VisualPanel::updateXYonMouse(const MouseEvent& e)
{
    if (bounds.expanded(boundsDelta).contains(e.position))
    {
        if (dimensions >= 1)
        {
            double r1 = jlimit(xSlider.getMinimum(), xSlider.getMaximum(),
                              double((e.position.getX()-bounds.getX()) / bounds.getWidth()));
            xSlider.setValue(r1);
        }
        if (dimensions == 2 || dimensions == 3)
        {
            double r2 = jlimit(ySlider.getMinimum(), ySlider.getMaximum(),
                              double((bounds.getBottom()-e.position.getY()) / bounds.getHeight()));
            ySlider.setValue(r2);
        }
    }
}

void VisualPanel::mouseDown(const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown() && bounds.expanded(boundsDelta).contains(e.position))
    {
        mouseDownInBounds = true;
        updateXYonMouse(e);
    }
    else
    {
        mouseDownInBounds = false;
    }
}

void VisualPanel::mouseDrag(const MouseEvent& e)
{
    if (mouseDownInBounds)
        updateXYonMouse(e);
}

void VisualPanel::mouseUp(const MouseEvent& /*e*/)
{
    if (mouseDownInBounds) mouseDownInBounds = false;
}

void VisualPanel::mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel)
{
    if (dimensions == 3)
    {
        MouseEvent eventNoDrag(e.source, e.position, e.mods.withoutMouseButtons(),
                               e.pressure, e.orientation, e.rotation, e.tiltX, e.tiltY,
                               e.eventComponent, e.originalComponent,
                               e.eventTime, e.position, e.eventTime, 0, false);
        zSlider.mouseWheelMove(eventNoDrag, wheel);
    }
    else
    {
        Component::mouseWheelMove(e, wheel);
    }
}
