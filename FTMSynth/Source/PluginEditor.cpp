/*
  ==============================================================================

    PluginEditor.cpp
    Created: 10 Oct 2019 10:03:03am
    Authors: Lily H, Loïc J

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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FTMSynthAudioProcessorEditor::FTMSynthAudioProcessorEditor(FTMSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), customLookAndFeel(), helpButton("help"),
      mainView(processor), midiConfigView(processor)
{
    setSize(640, 400);

    // Custom look and feel
    setLookAndFeel(&customLookAndFeel);
    tooltip->setLookAndFeel(&customLookAndFeel);
    tooltip->setOpaque(false);

    addAndMakeVisible(&mainView);
    addChildComponent(&midiConfigView);

    Image helpImg = ImageCache::getFromMemory(BinaryData::question_png, BinaryData::question_pngSize);
    Image helpOff = helpImg.getClippedImage(Rectangle<int>(0, 0, helpImg.getWidth()/2, helpImg.getHeight()));
    Image helpHovered = helpImg.getClippedImage(Rectangle<int>(helpImg.getWidth()/2, 0, helpImg.getWidth()/2, helpImg.getHeight()));
    helpButton.setImages(false, true, true,
                         helpOff, 1.0f, Colours::transparentBlack,
                         helpHovered, 1.0f, Colours::transparentBlack,
                         helpHovered, 1.0f, Colours::transparentBlack,
                         0.8f);
    helpButton.setTooltip("Information\n\nThis is a drum synth implemented with physical modeling. You may switch between the three available physical models - string, rectangular drum and cuboid box. Each of the knobs controls a combination of the underlying physical parameters, examined based on qualities of the sound produced.\n\n(Hover the mouse over the knob labels for more informations)");
    addAndMakeVisible(&helpButton);

    Image midiImg = ImageCache::getFromMemory(BinaryData::midi_png, BinaryData::midi_pngSize);
    Image midiOff = midiImg.getClippedImage(Rectangle<int>(0, 0, midiImg.getWidth()/3, midiImg.getHeight()));
    Image midiHovered = midiImg.getClippedImage(Rectangle<int>(midiImg.getWidth()/3, 0, midiImg.getWidth()/3, midiImg.getHeight()));
    Image midiOn = midiImg.getClippedImage(Rectangle<int>(midiImg.getWidth()*2/3, 0, midiImg.getWidth()/3, midiImg.getHeight()));
    midiButton.setImages(false, true, true,
                         midiOff, 1.0f, Colours::transparentBlack,
                         midiHovered, 1.0f, Colours::transparentBlack,
                         midiOn, 1.0f, Colours::transparentBlack,
                         0.8f);
    midiButton.setClickingTogglesState(true);
    midiButton.onStateChange = [this] { switchViews(); };
    midiButton.setTooltip("MIDI input settings");
    addAndMakeVisible(&midiButton);
}

FTMSynthAudioProcessorEditor::~FTMSynthAudioProcessorEditor()
{
    tooltip->setLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
}


//==============================================================================
void FTMSynthAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImage(background, Rectangle<float>(0, 0, 640, 400), RectanglePlacement::stretchToFit);
}

void FTMSynthAudioProcessorEditor::switchViews()
{
    if (midiButton.getToggleState())
    {
        mainView.setVisible(false);
        midiConfigView.setVisible(true);
    }
    else
    {
        midiConfigView.setVisible(false);
        mainView.setVisible(true);
    }
}

void FTMSynthAudioProcessorEditor::resized()
{
    setSize(640, 400);

    // show main and midi views here
    mainView.setBounds(0, 0, 640, 400);
    midiConfigView.setBounds(0, 0, 640, 480);

    helpButton.setBounds(16, 344, 48, 48);
    midiButton.setBounds(52, 344, 48, 48);
}
