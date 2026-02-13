/*
  ==============================================================================

    PluginProcessor.cpp
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
#include "../View/PluginEditor.h"

//==============================================================================
FTMSynthAudioProcessor::FTMSynthAudioProcessor()
    :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    tree(*this, nullptr, "Parameters",
    {
        std::make_unique<AudioParameterChoice>(ParameterID("algorithm", 1), "Algorithm", StringArray{"Ivan", "Rabenstein"}, 0),
        std::make_unique<AudioParameterFloat>(ParameterID("volume", 1), "Volume", NormalisableRange<float>(0.0f, 1.0f), 0.75f),
        std::make_unique<AudioParameterFloat>(ParameterID("attack", 1), "Attack",
            NormalisableRange<float>(0.0f, 4.0f,
                [](float start, float end, float normalizedSliderPos)
                {
                    float value = start + normalizedSliderPos * (end - start);
                    float nearestInt = std::round(value);
                    float snapWidth = 0.1f;

                    if (nearestInt < 1.0f || nearestInt > 3.0f) return value;
                    if (std::abs(value - nearestInt) < snapWidth) return nearestInt;

                    return value + (value > nearestInt ? -snapWidth : snapWidth);
                },
                [](float start, float end, float value)
                {
                    float nearestInt = std::round(value), snapWidth = 0.1f;
                    float valWithSnap = (nearestInt >= 1.0f && nearestInt <= 3.0f && value != nearestInt)
                                          ? value + (value > nearestInt ? snapWidth : -snapWidth)
                                          : value;
                    return (valWithSnap - start) / (end - start);
                }), 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID("pitch", 1), "Pitch", NormalisableRange<float>(-24.0f, 24.0f, 0.001f), 0.0f,  // in semitones
                                              AudioParameterFloatAttributes().withStringFromValueFunction([] (auto value, auto) { return String(value, 3); })),
        std::make_unique<AudioParameterBool>(ParameterID("kbTrack", 1), "Keyboard Tracking", true),
        std::make_unique<AudioParameterFloat>(ParameterID("sustain", 1), "Sustain", NormalisableRange<float>(0.01f, 0.8f, 0.0f, log(0.5f)/log(0.19f/0.79f)), 0.07f),
        std::make_unique<AudioParameterBool>(ParameterID("susGate", 1), "Sustain Gate", false),
        std::make_unique<AudioParameterFloat>(ParameterID("release", 1), "Release", NormalisableRange<float>(0.01f, 0.8f, 0.0f, log(0.5f)/log(0.19f/0.79f)), 0.07f),
        std::make_unique<AudioParameterFloat>(ParameterID("damp", 1), "Damp", NormalisableRange<float>(0.0f, 0.5f, 0.0f, log(0.5f)/log(0.1f/0.5f)), 0.0f),
        std::make_unique<AudioParameterBool>(ParameterID("dampGate", 1), "Damp Gate", false),
        std::make_unique<AudioParameterFloat>(ParameterID("ring", 1), "Ring", NormalisableRange<float>(0.0f, 0.5f, 0.0f, log(0.5f)/log(0.1f/0.5f)), 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID("dispersion", 1), "Inharmonicity", NormalisableRange<float>(0.0f, 5.0f, 0.0f, log(0.5f)/log(1.0f/5.0f)), 0.06f),
        std::make_unique<AudioParameterFloat>(ParameterID("squareness", 1), "Squareness", NormalisableRange<float>(0.01f, 1.0f), 0.5f),
        std::make_unique<AudioParameterFloat>(ParameterID("cubeness", 1), "Cubeness", NormalisableRange<float>(0.01f, 1.0f), 0.5f),
        std::make_unique<AudioParameterFloat>(ParameterID("r1", 1), "Impulse X", NormalisableRange<float>(0.005f, 0.995f), 0.5f),
        std::make_unique<AudioParameterFloat>(ParameterID("r2", 1), "Impulse Y", NormalisableRange<float>(0.005f, 0.995f), 0.5f),
        std::make_unique<AudioParameterFloat>(ParameterID("r3", 1), "Impulse Z", NormalisableRange<float>(0.005f, 0.995f), 0.5f),
        std::make_unique<AudioParameterInt>(ParameterID("m1", 1), "Modes X", 1, MAX_M1, 5),
        std::make_unique<AudioParameterInt>(ParameterID("m2", 1), "Modes Y", 1, MAX_M2, 5),
        std::make_unique<AudioParameterInt>(ParameterID("m3", 1), "Modes Z", 1, MAX_M3, 5),
        std::make_unique<AudioParameterInt>(ParameterID("dimensions", 1), "Dimensions", 1, 3, 2),
        std::make_unique<AudioParameterInt>(ParameterID("voices", 1), "Polyphony voices", 1, 16, 4)
    })
{
    SynthVoice::computeSinLUT();

    // clear and add voices
    mySynth.clearVoices();
    int numVoices = int(tree.getRawParameterValue("voices")->load());
    for (int i = 0; i < numVoices; i++)
    {
        mySynth.addVoice(new SynthVoice());
    }

    // clear and add sounds
    mySynth.clearSounds();
    mySynth.addSound(new SynthSound());

    // Initialize MIDI Mappings from shared table
    for (int i = 0; i < numMappableParams; ++i)
        midiMappings[paramTable[i].paramID] = std::make_unique<MidiMappingEntry>();

    loadGlobalMidiMappings();
}


FTMSynthAudioProcessor::~FTMSynthAudioProcessor()
{
}

//==============================================================================
const String FTMSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FTMSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FTMSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FTMSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FTMSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FTMSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FTMSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FTMSynthAudioProcessor::setCurrentProgram(int /*index*/)
{
}

const String FTMSynthAudioProcessor::getProgramName(int /*index*/)
{
    return {};
}

void FTMSynthAudioProcessor::changeProgramName(int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void FTMSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    ignoreUnused(samplesPerBlock);
    lastSampleRate=sampleRate;
    mySynth.setCurrentPlaybackSampleRate(lastSampleRate);
}

void FTMSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FTMSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
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

void FTMSynthAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    // Change the number of voices if needed
    int deltaVoices = int(tree.getRawParameterValue("voices")->load());
    deltaVoices -= mySynth.getNumVoices();

    if (deltaVoices > 0)
    {
        // Add new free voice
        for (int i = 0; i < deltaVoices; i++)
            mySynth.addVoice(new SynthVoice());
    }
    else if (deltaVoices < 0)
    {
        // Remove all found inactive voices starting from the end
        int removedVoices = 0;
        int voice = mySynth.getNumVoices()-1;
        while (voice > 0)
        {
            SynthVoice* myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(voice));
            if (myVoice != nullptr)
            {
                if (!myVoice->isVoiceActive())
                {
                    mySynth.removeVoice(voice);
                    removedVoices++;
                }
            }
            voice--;

            if (removedVoices >= (-deltaVoices)) break;
        }

        // If after a first pass, not enough voices were removed
        // then cut the least recent voices starting from the oldest
        if (removedVoices < (-deltaVoices))
        {
            SynthesiserVoice* voiceToCompare;
            int oldest;

            int leftToRemove = -deltaVoices;
            while (leftToRemove > 0)
            {
                oldest = -1;
                int voiceCount = mySynth.getNumVoices();

                for (int i = 0; i < voiceCount; i++)
                {
                    SynthVoice* myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i));
                    bool isOldest = true;  // until proven otherwise

                    for (int j = 0; j < voiceCount; j++)
                    {
                        if (i != j)
                        {
                            voiceToCompare = mySynth.getVoice(j);

                            if (myVoice != nullptr)
                            {
                                if (!myVoice->wasStartedBefore(*voiceToCompare))
                                    isOldest = false;
                            }
                        }
                    }
                    if (isOldest)
                    {
                        oldest = i;
                    }
                }
                if (oldest != -1)
                {
                    mySynth.removeVoice(oldest);
                    leftToRemove--;
                }
            }
        }
    }

    // Unified MIDI message processing
    MidiBuffer filteredMidi;
    int defaultCh = defaultChannel.load();

    for (const MidiMessageMetadata metadata : midiMessages)
    {
        MidiMessage message = metadata.getMessage();
        int inCh = message.getChannel() - 1;  // 0-15

        // --- Synth Events (Notes, Pitch, Aftertouch) ---
        // Filter by Default Channel
        if (message.isNoteOn() || message.isNoteOff() ||
            message.isPitchWheel() || message.isAftertouch() || message.isChannelPressure() ||
            message.isAllNotesOff() || message.isAllSoundOff())
        {
            if (defaultCh == -1 || defaultCh == inCh)
            {
                filteredMidi.addEvent(message, metadata.samplePosition);
            }
        }

        // --- CC Events (Parameter Mapping) ---
        else if (message.isController())
        {
            int inCC = message.getControllerNumber();

            // Learn Mode
            if (learningCC || learningChannel)
            {
                if (learningParamID.isNotEmpty())
                {
                    bool changed = false;
                    if (learningCC)
                    {
                        midiMappings[learningParamID]->cc.store(inCC);
                        learningCC = false;
                        changed = true;
                    }
                    if (learningChannel)
                    {
                        midiMappings[learningParamID]->channel.store(inCh);
                        learningChannel = false;
                        changed = true;
                    }

                    if (changed)
                    {
                        triggerAsyncUpdate();  // Safe save on Message Thread
                        sendChangeMessage();  // Notify view to update sliders/buttons
                        continue;  // Consume message, do not update parameter
                    }
                }
            }

            // Parameter Dispatch
            for (auto const& [paramID, entry] : midiMappings)
            {
                if (entry->cc.load() == inCC)
                {
                    bool channelMatch = false;
                    int mappedCh = entry->channel.load();

                    if (mappedCh == -1)  // OMNI
                        channelMatch = true;
                    else if (mappedCh == -2)  // MAIN
                    {
                        if (defaultCh == -1 || defaultCh == inCh)
                            channelMatch = true;
                    }
                    else if (mappedCh == inCh)  // Specific
                        channelMatch = true;

                    if (channelMatch)
                    {
                        RangedAudioParameter* param = tree.getParameter(paramID);
                        if (param != nullptr)
                        {
                            // Map 0-127 to 0.0-1.0
                            float newValue = (float)message.getControllerValue() / 127.0f;
                            param->setValueNotifyingHost(newValue);
                        }
                    }
                }
            }
        }
    }

    // Retrieve parameters from sliders and pass them to the model
    for (int i=0; i < mySynth.getNumVoices(); i++)
    {
        SynthVoice* myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i));
        if (myVoice != nullptr)
        {
            // this is the actual step that gets values from the tree, which are linked to the sliders
            // IMPORTANT NOTE: the parameters need to be passed from tree.getRawParameterValue("name"),
            //                 and NOT tree.getParameterAsValue("name").getValue(), otherwise they'll be
            //                 applied AFTER the next note press, instead of before, which means the
            //                 parameters will be updated one hit too late, which is what we *don't* want.
            myVoice->getcusParam(tree.getRawParameterValue("algorithm"),
                                 tree.getRawParameterValue("volume"),
                                 tree.getRawParameterValue("attack"),
                                 tree.getRawParameterValue("pitch"),
                                 tree.getRawParameterValue("kbTrack"),
                                 tree.getRawParameterValue("sustain"),
                                 tree.getRawParameterValue("susGate"),
                                 tree.getRawParameterValue("release"),
                                 tree.getRawParameterValue("damp"),
                                 tree.getRawParameterValue("dampGate"),
                                 tree.getRawParameterValue("ring"),
                                 tree.getRawParameterValue("dispersion"),
                                 tree.getRawParameterValue("squareness"),
                                 tree.getRawParameterValue("cubeness"),
                                 tree.getRawParameterValue("r1"),
                                 tree.getRawParameterValue("r2"),
                                 tree.getRawParameterValue("r3"),
                                 tree.getRawParameterValue("m1"),
                                 tree.getRawParameterValue("m2"),
                                 tree.getRawParameterValue("m3"),
                                 tree.getRawParameterValue("dimensions"));
        }
    }

    buffer.clear();

    mySynth.renderNextBlock(buffer, filteredMidi, 0, buffer.getNumSamples());
}

//==============================================================================
bool FTMSynthAudioProcessor::hasEditor() const
{
    return true;  // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FTMSynthAudioProcessor::createEditor()
{
    return new FTMSynthAudioProcessorEditor(*this);
}

//==============================================================================
void FTMSynthAudioProcessor::setMidiMapping(const String& paramID, int cc, int channel)
{
    if (midiMappings.find(paramID) != midiMappings.end())
    {
        midiMappings[paramID]->cc.store(cc);
        midiMappings[paramID]->channel.store(channel);
        saveGlobalMidiMappings();
    }
}

void FTMSynthAudioProcessor::setMidiLearn(const String& paramID, bool learnCC, bool learnChannel)
{
    if (paramID.isNotEmpty())
        learningParamID = paramID;

    if (learnCC) learningCC = !learningCC;
    if (learnChannel) learningChannel = !learningChannel;

    sendChangeMessage();  // Notify view of state change
}

void FTMSynthAudioProcessor::handleAsyncUpdate()
{
    saveGlobalMidiMappings();
}

//==============================================================================
PropertiesFile::Options FTMSynthAudioProcessor::getGlobalSettingsOptions()
{
    PropertiesFile::Options options;
    options.applicationName = JucePlugin_Name;
    options.filenameSuffix = ".settings";
    options.folderName = JucePlugin_Name;
    options.osxLibrarySubFolder = "Application Support";
    return options;
}

void FTMSynthAudioProcessor::saveGlobalMidiMappings()
{
    ApplicationProperties props;
    props.setStorageParameters(getGlobalSettingsOptions());

    if (auto xml = getMidiMappingsAsXml())
    {
        props.getUserSettings()->setValue("midi-mapping", xml->toString());
        props.saveIfNeeded();
    }
}

void FTMSynthAudioProcessor::loadGlobalMidiMappings()
{
    ApplicationProperties props;
    props.setStorageParameters(getGlobalSettingsOptions());

    String xmlString = props.getUserSettings()->getValue("midi-mapping");
    if (xmlString.isNotEmpty())
    {
        std::unique_ptr<XmlElement> xml(XmlDocument::parse(xmlString));
        if (xml != nullptr)
        {
            restoreMidiMappingsFromXml(*xml);
        }
    }
}

std::unique_ptr<XmlElement> FTMSynthAudioProcessor::getMidiMappingsAsXml()
{
    auto xml = std::make_unique<XmlElement>("midiconfig");

    // Default Channel
    xml->setAttribute("mainchannel", defaultChannel.load());

    for (auto const& [id, entry] : midiMappings)
    {
        XmlElement* e = xml->createNewChildElement("mapping");
        e->setAttribute("id", id);
        e->setAttribute("cc", entry->cc.load());
        e->setAttribute("channel", entry->channel.load());
    }

    return xml;
}

void FTMSynthAudioProcessor::restoreMidiMappingsFromXml(const XmlElement& xml)
{
    if (xml.hasTagName("midiconfig"))
    {
        defaultChannel.store(xml.getIntAttribute("mainchannel", -1));

        for (auto* child : xml.getChildIterator())
        {
            if (child->hasTagName("mapping"))
            {
                String id = child->getStringAttribute("id");
                if (midiMappings.find(id) != midiMappings.end())
                {
                    midiMappings[id]->cc.store(child->getIntAttribute("cc"));
                    midiMappings[id]->channel.store(child->getIntAttribute("channel"));
                }
            }
        }
    }
}

//==============================================================================
void FTMSynthAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    auto root = std::make_unique<XmlElement>("FTMSynthState");

    // Patch parameters
    root->addChildElement(tree.copyState().createXml().release());

    // MIDI mapping meta-parameters
    root->addChildElement(getMidiMappingsAsXml().release());

    copyXmlToBinary(*root, destData);
}

void FTMSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto root = getXmlFromBinary(data, sizeInBytes);
    if (root == nullptr) return;

    if (root->hasTagName("FTMSynthState"))
    {
        // New format: root wrapper with children
        if (auto* paramXml = root->getChildByName(tree.state.getType()))
            tree.replaceState(ValueTree::fromXml(*paramXml));

        if (auto* midiXml = root->getChildByName("midiconfig"))
            restoreMidiMappingsFromXml(*midiXml);
    }
    else if (root->hasTagName(tree.state.getType()))
    {
        // Legacy format: just APVTS state (backward compat)
        tree.replaceState(ValueTree::fromXml(*root));
    }
}

void FTMSynthAudioProcessor::resetAllParametersToDefault()
{
    for (auto* param : getParameters())
    {
        if (auto* rangedParam = dynamic_cast<RangedAudioParameter*>(param))
            rangedParam->setValueNotifyingHost(rangedParam->getDefaultValue());
    }
}

//==============================================================================
// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FTMSynthAudioProcessor();
}
