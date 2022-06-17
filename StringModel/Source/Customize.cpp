/*
  ==============================================================================

    Customize.cpp
    Created: 10 Oct 2019 10:02:42am
    Author:  Lily H

  ==============================================================================
*/

#include "Customize.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "Customize.h"

//==============================================================================
Customize::Customize(StringModelAudioProcessor& p,int mode):
processor(p),helpComp("help")
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    Image helpImg = ImageCache::getFromMemory(BinaryData::question2_png, BinaryData::question2_pngSize);

    helpComp.setImage(helpImg);
    helpComp.setTooltip("Information\n This is a drum synth implemented with physical modeling. You may switch between the three available physical models - string, rectangular drum and cuboid box. Each of the knobs controls a combination of the underlying physical parameters, examined based on qualities of the sound produced.\n Sustain - short to long\n Roundness - harsh to round\n Inharmonicity - harmonic to inharmonic\n Drum squareness - elongated to square drum\n Box shape - flat to cubic box");
    addAndMakeVisible(&helpComp);

    //Font myFont = Font(Typeface::createSystemTypefaceFor(BinaryData::AmaticSCRegular_ttf, BinaryData::AmaticSCRegular_ttfSize));
    //Font myFont2 = Font(Typeface::createSystemTypefaceFor(BinaryData::ColabThi_otf,
    //BinaryData::ColabThi_otfSize));
    //myFont2.setBold(true);
    //Font myFont3 = Font(Typeface::createSystemTypefaceFor(BinaryData::ExistenceLight_otf,
    //BinaryData::ExistenceLight_otfSize));

    tauSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    tauSlider.setRange(0.1f,5000.0f);
    tauSlider.setValue(0.1f);
    //tauSlider.setTextValueSuffix (" s");
    tauSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
    tauSlider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
    addAndMakeVisible(&tauSlider);
    addAndMakeVisible (&tauLabel);
    tauLabel.setText ("Sustain", dontSendNotification);//(fundamental time constant)
    //tauLabel.setFont(myFont2);
    //tauLabel.attachToComponent (&tauSlider, false);
    tauTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"sustain",tauSlider)); //this class maintains a connection between slider and parameter in apvts

    //omegaTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"omega",omegaSlider));

    pSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    pSlider.setRange(0.1f,5000.0f);
    pSlider.setValue(0.1f);
    pSlider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
    addAndMakeVisible(&pSlider);
    addAndMakeVisible (&pLabel);
    pSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
    pLabel.setText ("Roundness", dontSendNotification);//(frequency dependent damping)
    //pLabel.setFont(myFont3);
    //pLabel.attachToComponent (&pSlider, false);
    pTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"damp",pSlider));

    dispersionSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dispersionSlider.setRange(0.1f,5000.0f);
    dispersionSlider.setValue(0.1f);
    dispersionSlider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
    addAndMakeVisible(&dispersionSlider);
    addAndMakeVisible (&dispersionLabel);
    dispersionSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
    dispersionLabel.setText ("Inharmonicity", dontSendNotification);//(inharmonicity)
    //dispersionLabel.attachToComponent (&dispersionSlider, false);
    dispersionTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"dispersion",dispersionSlider));

    r1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    r1Slider.setRange(0.0f, 1.0f);
    r1Slider.setValue(0.5f);
    r1Slider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
    addAndMakeVisible(&r1Slider);
    addAndMakeVisible (&r1Label);
    r1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
    r1Label.setText ("Impulse X", dontSendNotification);
    //r1Label.attachToComponent (&r1Slider, false);
    r1Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"r1",r1Slider));

    if(mode >= 2){
        alpha1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        alpha1Slider.setRange(0.1f,5000.0f);
        alpha1Slider.setValue(0.1f);
        alpha1Slider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
        addAndMakeVisible(&alpha1Slider);
        addAndMakeVisible (&alpha1Label);
        alpha1Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
        alpha1Label.setText ("Drum Squareness", dontSendNotification);//(shape of drum)
        //std::cout<<"fonts "<<juce::Font::findAllTypefaceNames()[0]<<"\n";
        //alpha1Label.setFont(myFont);
        //alpha1Label.attachToComponent (&alpha1Slider, false);
        alphaTree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"squareness",alpha1Slider));

        r2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        r2Slider.setRange(0.0f, 1.0f);
        r2Slider.setValue(0.5f);
        r2Slider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
        addAndMakeVisible(&r2Slider);
        addAndMakeVisible (&r2Label);
        r2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
        r2Label.setText ("Impulse Y", dontSendNotification);
        //r2Label.attachToComponent (&r2Slider, false);
        r2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"r2",r2Slider));

        if(mode == 3){
            alpha2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
            alpha2Slider.setRange(0.1f,5000.0f);
            alpha2Slider.setValue(0.1f);
            alpha2Slider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
            addAndMakeVisible(&alpha2Slider);
            addAndMakeVisible (&alpha2Label);
            alpha2Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
            alpha2Label.setText ("Box Shape", dontSendNotification); // (shape of drum)
            //alpha2Label.attachToComponent (&alpha2Slider, false);
            alpha2Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"cubeness",alpha2Slider));

            r3Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
            r3Slider.setRange(0.0f, 1.0f);
            r3Slider.setValue(0.5f);
            r3Slider.setColour(Slider::ColourIds::backgroundColourId,Colours::red);
            addAndMakeVisible(&r3Slider);
            addAndMakeVisible (&r3Label);
            r3Slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,true,0,0);
            r3Label.setText ("Impulse Z", dontSendNotification);
            //r3Label.attachToComponent (&r3Slider, false);
            r3Tree.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree,"r3",r3Slider));
        }
    }
}

Customize::~Customize()
{
}

void Customize::paint(Graphics& g)
{
    //g.fillAll (Colours::white);
    cusSlider* cusRotarySliderlook = new cusSlider();
    if(auto* newl = dynamic_cast<juce::LookAndFeel*> (cusRotarySliderlook))
    {
        tauSlider.setLookAndFeel(newl);
        pSlider.setLookAndFeel(newl);
        dispersionSlider.setLookAndFeel(newl);
        alpha1Slider.setLookAndFeel(newl);
        alpha2Slider.setLookAndFeel(newl);
        r1Slider.setLookAndFeel(newl);
        r2Slider.setLookAndFeel(newl);
        r3Slider.setLookAndFeel(newl);
    }
}

void Customize::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    //Rectangle<int> area=getLocalBounds();

    tauSlider.setBounds(13,30,100,100);
    tauLabel.setBounds(37,131,133,11);
    pSlider.setBounds(125,103,100,100);
    pLabel.setBounds(136,205,133,11);
    dispersionSlider.setBounds(13,177,100,100);
    dispersionLabel.setBounds(18,279,133,13);
    alpha1Slider.setBounds(125,247,100,100);
    alpha1Label.setBounds(119,351,131,13);
    alpha2Slider.setBounds(13,321,100,100);
    alpha2Label.setBounds(29,426,133,13);
    r1Slider.setBounds(395,30,100,100);
    r1Label.setBounds(407,131,133,13);
    r2Slider.setBounds(395,177,100,100);
    r2Label.setBounds(407,279,133,13);
    r3Slider.setBounds(395,321,100,100);
    r3Label.setBounds(407,426,133,13);
    helpComp.setBounds(2,447,23,23);
    //dimMenu.setBounds(0,1500,100,100);
}

// seq: 1. tau, 2. p, 3. dispersion, 4. alpha, 5. alpha2, 6. r1, 7. r2, 8. r3
double Customize::getsliderval(int seq)
{
    if (seq == 1)
    {
        return tauSlider.getValue();
    }
    else if (seq == 2)
    {
        return pSlider.getValue();
    }
    else if (seq == 3)
    {
        return dispersionSlider.getValue();
    }
    else if (seq == 4)
    {
        return alpha1Slider.getValue();
    }
    else if (seq == 5)
    {
        return alpha2Slider.getValue();
    }
    else if (seq == 6)
    {
        return r1Slider.getValue();
    }
    else if (seq == 7)
    {
        return r2Slider.getValue();
    }
    else if (seq == 8)
    {
        return r3Slider.getValue();
    }

    return 0;
}
