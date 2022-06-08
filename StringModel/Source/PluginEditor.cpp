/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StringModelAudioProcessorEditor::StringModelAudioProcessorEditor (StringModelAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),mode(TabbedButtonBar::Orientation::TabsAtRight),
stringButton("string", DrawableButton::ButtonStyle::ImageStretched ),
drumButton("drum", DrawableButton::ButtonStyle::ImageStretched ),
boxButton("box", DrawableButton::ButtonStyle::ImageStretched ),
stringCUS(p,1),drumCUS(p,2),boxCUS(p,3)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (520,500);
 
    std::unique_ptr<Drawable> buttonOff;
    std::unique_ptr<Drawable> buttonOn;
     
    buttonOff = Drawable::createFromImageData(BinaryData::stringOff_png, BinaryData::stringOff_pngSize);
    buttonOn = Drawable::createFromImageData(BinaryData::stringOn_png, BinaryData::stringOn_pngSize);
    stringButton.setClickingTogglesState(true);
    stringButton.setImages(buttonOff.get(), nullptr,nullptr,nullptr,buttonOn.get());
    stringButton.onStateChange = [this] {buttonClicked(1);};
    stringButton.setRadioGroupId(1);

    stringTree = new AudioProcessorValueTreeState::ButtonAttachment(processor.tree,"dim1",stringButton);

     addAndMakeVisible (stringButton);
       
    buttonOff = Drawable::createFromImageData(BinaryData::drumOff_png, BinaryData::drumOff_pngSize);
    buttonOn = Drawable::createFromImageData(BinaryData::drumOn_png, BinaryData::drumOn_pngSize);
    drumButton.setClickingTogglesState(true);
    drumButton.setImages(buttonOff.get(), nullptr,nullptr,nullptr,buttonOn.get());
    drumButton.onStateChange = [this] {buttonClicked(2);};
    drumButton.setRadioGroupId(1);
    drumTree = new AudioProcessorValueTreeState::ButtonAttachment(processor.tree,"dim2",drumButton);

    addAndMakeVisible (drumButton);
    
    buttonOff = Drawable::createFromImageData(BinaryData::boxOff_png, BinaryData::boxOff_pngSize);
    buttonOn = Drawable::createFromImageData(BinaryData::boxOn_png, BinaryData::boxOn_pngSize);
    boxButton.setClickingTogglesState(true);
    boxButton.setImages(buttonOff.get(), nullptr,nullptr,nullptr,buttonOn.get());
    boxButton.onStateChange = [this] {buttonClicked(3);};
    boxButton.setRadioGroupId(1);
    boxTree = new AudioProcessorValueTreeState::ButtonAttachment(processor.tree,"dim3",boxButton);

    addAndMakeVisible (boxButton);
    
    
    if(stringButton.getToggleState()==1){
        stringCUS.setVisible(true);
    }
    else if(drumButton.getToggleState()==1){
        drumCUS.setVisible(true);
    }
    else if(boxButton.getToggleState()==1){
        boxCUS.setVisible(true);
    }
    addChildComponent(stringCUS, 0);
    addChildComponent(drumCUS, 0);
    addChildComponent(boxCUS, 0);
}

StringModelAudioProcessorEditor::~StringModelAudioProcessorEditor()
{
}

//==============================================================================
void StringModelAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    RectanglePlacement backgroundRectanglePlacement(64);
    g.drawImageWithin(background, 0, 0,getWidth(),getHeight(),backgroundRectanglePlacement,false);
    if(stringButton.getToggleState()==1){
        stringCUS.setBounds(0,0,496,479);
        drumCUS.setVisible(false);
        boxCUS.setVisible(false);
        stringCUS.setVisible(true);
   }
    else if(drumButton.getToggleState()==1){
        drumCUS.setBounds(0,0,496,479);
        stringCUS.setVisible(false);
        boxCUS.setVisible(false);
        drumCUS.setVisible(true);
    }
    else if(boxButton.getToggleState()==1){
        boxCUS.setBounds(0,0,496,479);
        drumCUS.setVisible(false);
        stringCUS.setVisible(false);
        boxCUS.setVisible(true);
    }
}

void StringModelAudioProcessorEditor::buttonClicked(int seq)
{
    if(seq==1){
        stringCUS.repaint();
        drumCUS.setVisible(false);
        boxCUS.setVisible(false);
    }
    else if(seq==2){
        drumCUS.repaint();
        stringCUS.setVisible(false);
        boxCUS.setVisible(false);
    }
    else if(seq==3){
        boxCUS.repaint();
        drumCUS.setVisible(false);
        stringCUS.setVisible(false);
    }
}

void StringModelAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    setSize (512,470);

    stringButton.setBounds(249,10,122,122);
    drumButton.setBounds(249,169,122,122);
    boxButton.setBounds(249,326,122,122);
}
