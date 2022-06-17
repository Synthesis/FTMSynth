/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StringModelAudioProcessor::StringModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
tree (*this,nullptr,"PARAMETERS",
{   std::make_unique<AudioParameterFloat> ("tau", "tau", NormalisableRange<float> (0.01f,0.3f),0.07f),
    std::make_unique<AudioParameterFloat> ("p", "p", NormalisableRange<float> (0.0f,0.35f),0.0f),
    std::make_unique<AudioParameterFloat> ("dispersion", "dispersion", NormalisableRange<float> (0.0f,10.0f),0.06f),
    std::make_unique<AudioParameterFloat> ("alpha1", "alpha1", NormalisableRange<float> (0.01f,1.0f),0.5f),
    std::make_unique<AudioParameterFloat> ("alpha2", "alpha2", NormalisableRange<float> (0.01f,1.0f),0.5f),
    //std::make_unique<AudioParameterInt> ("dimtype", "dimtype", 0,2,1),
    std::make_unique<AudioParameterInt> ("dim1", "dim1", 0,1,0),
    std::make_unique<AudioParameterInt> ("dim2", "dim2", 0,1,1),
    std::make_unique<AudioParameterInt> ("dim3", "dim3", 0,1,0),
    std::make_unique<AudioParameterFloat> ("r1", "r1", NormalisableRange<float> (0.0f,1.0f), 0.5f),
    std::make_unique<AudioParameterFloat> ("r2", "r2", NormalisableRange<float> (0.0f,1.0f), 0.5f),
    std::make_unique<AudioParameterFloat> ("r3", "r3", NormalisableRange<float> (0.0f,1.0f), 0.5f)
})
{
    mySynth.clearVoices();
    for(int i=0;i<1;i++){
        
        mySynth.addVoice(new SynthVoice());
    }
    //clear and add sounds
    mySynth.clearSounds();
    mySynth.addSound(new SynthSound());
}



StringModelAudioProcessor::~StringModelAudioProcessor()
{
}

//==============================================================================
const String StringModelAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StringModelAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StringModelAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StringModelAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StringModelAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StringModelAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StringModelAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StringModelAudioProcessor::setCurrentProgram (int index)
{
}

const String StringModelAudioProcessor::getProgramName (int index)
{
    return {};
}

void StringModelAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void StringModelAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    ignoreUnused(samplesPerBlock);
    lastSampleRate=sampleRate;
    mySynth.setCurrentPlaybackSampleRate(lastSampleRate);
}

void StringModelAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StringModelAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StringModelAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    for(int i=0;i<mySynth.getNumVoices();i++)
    {
        //a mechanism to get parameters from slider and pass to adsr
        if((myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i))))
        {
            //add my synthesizer, inputs are values from my new sliders
            myVoice->getcusParam(tree.getRawParameterValue("tau"),//this is the actual step that gets value from the tree, which are linked with slider
                                 tree.getRawParameterValue("omega"),
                                 tree.getRawParameterValue("p"),
                                 tree.getRawParameterValue("dispersion"),
                                 tree.getRawParameterValue("alpha1"),
                                 tree.getRawParameterValue("alpha2"),
                                 tree.getRawParameterValue("r1"),
                                 tree.getRawParameterValue("r2"),
                                 tree.getRawParameterValue("r3"),
                                 tree.getRawParameterValue("dim1"),
                                 tree.getRawParameterValue("dim2"),
                                 tree.getRawParameterValue("dim3"));
        }
    }
    
    buffer.clear();
    MidiBuffer processedMidi;//can modify this processMidi and swap with the original midi
    //MidiBuffer processedMidi;//can modify this processMidi and swap with the original midi
    
    MidiMessage m;
    //MidiMessage m;
    
    mySynth.renderNextBlock(buffer, midiMessages , 0, buffer.getNumSamples());//which is the callback function
}

//==============================================================================
bool StringModelAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StringModelAudioProcessor::createEditor()
{
    return new StringModelAudioProcessorEditor (*this);
}

//==============================================================================
void StringModelAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = tree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void StringModelAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(tree.state.getType()))
            tree.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StringModelAudioProcessor();
}
