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

#include "PluginEditor.h"
#include "juce_core/juce_core.h"
#include <memory>

#define PRESET_EXTENSION ".ftmpreset"

//==============================================================================
FTMSynthAudioProcessorEditor::FTMSynthAudioProcessorEditor(FTMSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), customLookAndFeel(), helpButton("help"),
      labelView(processor), mainView(processor), midiConfigView(processor)
{
    setSize(640, 400);

    // Custom look and feel
    setLookAndFeel(&customLookAndFeel);
    tooltip->setLookAndFeel(&customLookAndFeel);
    tooltip->setOpaque(false);

    // Panels
    labelView.setOpaqueLabels(false);
    addAndMakeVisible(labelView);
    addAndMakeVisible(mainView);
    addChildComponent(midiConfigView);

    // Buttons
    Image helpImg = ImageCache::getFromMemory(BinaryData::question_png, BinaryData::question_pngSize);
    Image helpOff = helpImg.getClippedImage(Rectangle<int>(0, 0, helpImg.getWidth()/3, helpImg.getHeight()));
    Image helpHovered = helpImg.getClippedImage(Rectangle<int>(helpImg.getWidth()/3, 0, helpImg.getWidth()/3, helpImg.getHeight()));
    Image helpOn = helpImg.getClippedImage(Rectangle<int>(helpImg.getWidth()*2/3, 0, helpImg.getWidth()/3, helpImg.getHeight()));
    helpButton.setImages(false, true, true,
                         helpOff, 1.0f, Colours::transparentBlack,
                         helpHovered, 1.0f, Colours::transparentBlack,
                         helpOn, 1.0f, Colours::transparentBlack,
                         0.8f);
    helpButton.setClickingTogglesState(true);
    helpButton.onClick = [this] { mainView.showHelp(helpButton.getToggleState()); };
    helpButton.setTooltip("About FTMSynth");
    addAndMakeVisible(helpButton);

    Image presetFileImg = ImageCache::getFromMemory(BinaryData::savePreset_png, BinaryData::savePreset_pngSize);
    Image presetFileOff = presetFileImg.getClippedImage(Rectangle<int>(0, 0, presetFileImg.getWidth()/3, presetFileImg.getHeight()));
    Image presetFileHovered = presetFileImg.getClippedImage(Rectangle<int>(presetFileImg.getWidth()/3, 0, presetFileImg.getWidth()/3, presetFileImg.getHeight()));
    Image presetFileDown = presetFileImg.getClippedImage(Rectangle<int>(presetFileImg.getWidth()*2/3, 0, presetFileImg.getWidth()/3, presetFileImg.getHeight()));
    presetFileButton.setImages(false, true, true,
                               presetFileOff, 1.0f, Colours::transparentBlack,
                               presetFileHovered, 1.0f, Colours::transparentBlack,
                               presetFileDown, 1.0f, Colours::transparentBlack,
                               0.8f);
    presetFileButton.setClickingTogglesState(false);
    presetFileButton.onClick = [&]
    {
        auto menu = std::make_shared<PopupMenu>();
        menu->setLookAndFeel(&customLookAndFeel);
        menu->addItem("Init",
            [this] {
                NativeMessageBox::showYesNoBox(
                    AlertWindow::WarningIcon,
                    "Reinitialize patch",
                    "Reset current patch to default?",
                    this,
                    ModalCallbackFunction::create([this](int result)
                    {
                        if (result != 0)  // Yes
                        {
                            processor.resetAllParametersToDefault();
                        }
                    }));
            });
        menu->addItem("Open preset",
            [this] {
                auto fc = std::make_shared<FileChooser>(
                    "Open preset",
                    File::getSpecialLocation(File::userHomeDirectory),
                    PRESET_EXTENSION);

                fc->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                [this, fc](const FileChooser& chooser)
                                {
                                    File result = chooser.getResult();
                                    if (result != File())
                                    {
                                        XmlDocument doc(result);
                                        if (auto xml = doc.getDocumentElement())
                                        {
                                            if (xml->hasTagName(processor.tree.state.getType()))
                                            {
                                                processor.tree.replaceState(ValueTree::fromXml(*xml));
                                            }
                                        }
                                    }
                                });
            });
        menu->addItem("Save preset",
            [this] {
                auto fc = std::make_shared<FileChooser>(
                    "Save preset",
                    File::getSpecialLocation(File::userHomeDirectory),
                    PRESET_EXTENSION);

                fc->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting,
                                [this, fc](const FileChooser& chooser)
                                {
                                    File result = chooser.getResult();
                                    if (result != File())
                                    {
                                        // Ensure extension
                                        if (!result.hasFileExtension(PRESET_EXTENSION))
                                            result = result.withFileExtension(PRESET_EXTENSION);

                                        std::unique_ptr<XmlElement> xml(processor.tree.copyState().createXml());
                                        xml->writeTo(result);
                                    }
                                });
            });
        menu->showMenuAsync(PopupMenu::Options().withTargetScreenArea(presetFileButton.getScreenBounds().reduced(8)));
    };
    presetFileButton.setTooltip("Load/Save preset");
    addAndMakeVisible(presetFileButton);

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
    midiButton.onClick = [this] { switchViews(); };
    midiButton.setTooltip("MIDI input settings");
    addAndMakeVisible(midiButton);
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
        helpButton.setEnabled(false);
        labelView.setOpaqueLabels(true);
        mainView.setVisible(false);
        midiConfigView.setVisible(true);
    }
    else
    {
        helpButton.setEnabled(true);
        labelView.setOpaqueLabels(false);
        midiConfigView.setVisible(false);
        mainView.setVisible(true);
    }
}

void FTMSynthAudioProcessorEditor::resized()
{
    setSize(640, 400);

    helpButton.setBounds(      16, 344, 48, 48);
    presetFileButton.setBounds(48, 344, 48, 48);
    midiButton.setBounds(      80, 344, 48, 48);
}
