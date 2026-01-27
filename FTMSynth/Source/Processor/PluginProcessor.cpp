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
        std::make_unique<AudioParameterChoice>("algorithm", "Algorithm", StringArray{"Ivan", "Rabenstein"}, 0),
        std::make_unique<AudioParameterFloat>("volume", "Volume", NormalisableRange<float>(0.0f, 1.0f), 0.75f),
        std::make_unique<AudioParameterFloat>("attack", "Attack", NormalisableRange<float>(0.0f, 1.0f), 1.0f),
        std::make_unique<AudioParameterFloat>("pitch", "Pitch", NormalisableRange<float>(-24.0f, 24.0f, 0.001f), 0.0f,  // in semitones
                                              AudioParameterFloatAttributes().withStringFromValueFunction([] (auto value, auto) { return String(value, 3); })),
        std::make_unique<AudioParameterBool>("kbTrack", "Keyboard Tracking", true),
        std::make_unique<AudioParameterFloat>("sustain", "Sustain", NormalisableRange<float>(0.01f, 0.8f, 0.0f, log(0.5f)/log(0.19f/0.79f)), 0.07f),
        std::make_unique<AudioParameterBool>("susGate", "Sustain Gate", false),
        std::make_unique<AudioParameterFloat>("release", "Release", NormalisableRange<float>(0.01f, 0.8f, 0.0f, log(0.5f)/log(0.19f/0.79f)), 0.07f),
        std::make_unique<AudioParameterFloat>("damp", "Damp", NormalisableRange<float>(0.0f, 0.5f, 0.0f, log(0.5f)/log(0.1f/0.5f)), 0.0f),
        std::make_unique<AudioParameterBool>("dampGate", "Damp Gate", false),
        std::make_unique<AudioParameterFloat>("ring", "Ring", NormalisableRange<float>(0.0f, 0.5f, 0.0f, log(0.5f)/log(0.1f/0.5f)), 0.0f),
        std::make_unique<AudioParameterFloat>("dispersion", "Inharmonicity", NormalisableRange<float>(0.0f, 5.0f, 0.0f, log(0.5f)/log(1.0f/5.0f)), 0.06f),
        std::make_unique<AudioParameterFloat>("squareness", "Squareness", NormalisableRange<float>(0.01f, 1.0f), 0.5f),
        std::make_unique<AudioParameterFloat>("cubeness", "Cubeness", NormalisableRange<float>(0.01f, 1.0f), 0.5f),
        std::make_unique<AudioParameterFloat>("r1", "Impulse X", NormalisableRange<float>(0.005f, 0.995f), 0.5f),
        std::make_unique<AudioParameterFloat>("r2", "Impulse Y", NormalisableRange<float>(0.005f, 0.995f), 0.5f),
        std::make_unique<AudioParameterFloat>("r3", "Impulse Z", NormalisableRange<float>(0.005f, 0.995f), 0.5f),
        std::make_unique<AudioParameterInt>("m1", "Modes X", 1, MAX_M1, 5),
        std::make_unique<AudioParameterInt>("m2", "Modes Y", 1, MAX_M2, 5),
        std::make_unique<AudioParameterInt>("m3", "Modes Z", 1, MAX_M3, 5),
        std::make_unique<AudioParameterInt>("dimensions", "Dimensions", 1, 3, 2),
        std::make_unique<AudioParameterInt>("voices", "Polyphony voices", 1, 16, 4)
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

#if JucePlugin_Build_Standalone
    // Helper to create mapping entry
    auto createMapping = [](String id)
    {
        auto entry = std::make_unique<MidiMappingEntry>();

        entry->cc = std::make_unique<AudioParameterInt>(id + "_cc", "CC", -1, 127, -1);
        entry->channel = std::make_unique<AudioParameterInt>(id + "_ch", "Channel", -2, 15, -2);

        return entry;
    };

    // Initialize MIDI Mappings
    midiMappings["volume"] = createMapping("volume");
    midiMappings["attack"] = createMapping("attack");
    midiMappings["dimensions"] = createMapping("dimensions");
    midiMappings["pitch"] = createMapping("pitch");
    midiMappings["sustain"] = createMapping("sustain");
    midiMappings["damp"] = createMapping("damp");
    midiMappings["dispersion"] = createMapping("dispersion");
    midiMappings["squareness"] = createMapping("squareness");
    midiMappings["cubeness"] = createMapping("cubeness");
    midiMappings["r1"] = createMapping("r1");
    midiMappings["r2"] = createMapping("r2");
    midiMappings["r3"] = createMapping("r3");
    midiMappings["m1"] = createMapping("m1");
    midiMappings["m2"] = createMapping("m2");
    midiMappings["m3"] = createMapping("m3");
    midiMappings["voices"] = createMapping("voices");
    midiMappings["algorithm"] = createMapping("algorithm");

    defaultChannelParam = std::make_unique<AudioParameterInt>("default_ch", "Default Channel", -1, 15, -1);

    loadGlobalMidiMappings();
#endif
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
            myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(voice));
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
                    myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i));
                    bool isOldest = true; // until proven otherwise

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

#if JucePlugin_Build_Standalone
    // Unified MIDI message processing (Standalone only)
    MidiBuffer filteredMidi;
    int defaultCh = defaultChannelParam->get();

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        int inCh = message.getChannel() - 1; // 0-15

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
                        *midiMappings[learningParamID]->cc = inCC;
                        learningCC = false;
                        changed = true;
                    }
                    if (learningChannel)
                    {
                        *midiMappings[learningParamID]->channel = inCh;
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
                if (entry->cc->get() == inCC)
                {
                    bool channelMatch = false;
                    int mappedCh = entry->channel->get();

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
                        auto* param = tree.getParameter(paramID);
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
#endif

    // Retrieve parameters from sliders and pass them to the model
    for (int i=0; i < mySynth.getNumVoices(); i++)
    {
        myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i));
        if (myVoice != nullptr)
        {
            // this is the actual step that gets values from the tree, which are linked to the sliders
            // IMPORTANT NOTE: the parameters need to be passed from tree.getRawParameterValue("name"),
            //                 and NOT tree.getParameterAsValue("name").getValue(), otherwise they'll be
            //                 applied AFTER the next note press, instead of before, which means the
            //                 parameters will be updated one hit too late, which is what we *don't* want.
            myVoice->getcusParam(tree.getRawParameterValue("algorithm"),
                                 tree.getRawParameterValue("volume"),
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

#if JucePlugin_Build_Standalone
    mySynth.renderNextBlock(buffer, filteredMidi, 0, buffer.getNumSamples());
#else
    mySynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
#endif
}

//==============================================================================
bool FTMSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FTMSynthAudioProcessor::createEditor()
{
    return new FTMSynthAudioProcessorEditor(*this);
}

//==============================================================================
#if JucePlugin_Build_Standalone
void FTMSynthAudioProcessor::setMidiMapping(String paramID, int cc, int channel)
{
    if (midiMappings.find(paramID) != midiMappings.end())
    {
        *midiMappings[paramID]->cc = cc;
        *midiMappings[paramID]->channel = channel;
        saveGlobalMidiMappings();
    }
}

void FTMSynthAudioProcessor::setMidiLearn(String paramID, bool learnCC, bool learnChannel)
{
    if (paramID.isNotEmpty())
        learningParamID = paramID;

    if (learnCC) learningCC = !learningCC;
    if (learnChannel) learningChannel = !learningChannel;

    sendChangeMessage();  // Notify view of state change
}

MidiMapping FTMSynthAudioProcessor::getMidiMapping(String paramID)
{
    MidiMapping m;
    if (midiMappings.find(paramID) != midiMappings.end())
    {
        m.cc = midiMappings[paramID]->cc->get();
        m.channel = midiMappings[paramID]->channel->get();
    }
    return m;
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

    std::unique_ptr<XmlElement> xml(new XmlElement("MidiMappings"));

    // Default Channel
    xml->setAttribute("defaultChannel", defaultChannelParam->get());

    for (auto const& [id, entry] : midiMappings)
    {
        auto* e = xml->createNewChildElement("Mapping");
        e->setAttribute("paramID", id);
        e->setAttribute("cc", entry->cc->get());
        e->setAttribute("channel", entry->channel->get());
    }

    props.getUserSettings()->setValue("MidiMappings", xml->toString());
    props.saveIfNeeded();
}

void FTMSynthAudioProcessor::loadGlobalMidiMappings()
{
    ApplicationProperties props;
    props.setStorageParameters(getGlobalSettingsOptions());

    auto xmlString = props.getUserSettings()->getValue("MidiMappings");
    if (xmlString.isNotEmpty())
    {
        std::unique_ptr<XmlElement> xml(XmlDocument::parse(xmlString));
        if (xml != nullptr && xml->hasTagName("MidiMappings"))
        {
            *defaultChannelParam = xml->getIntAttribute("defaultChannel", -1);

            for (auto* child : xml->getChildIterator())
            {
                if (child->hasTagName("Mapping"))
                {
                    String id = child->getStringAttribute("paramID");
                    if (midiMappings.find(id) != midiMappings.end())
                    {
                        *midiMappings[id]->cc = child->getIntAttribute("cc");
                        *midiMappings[id]->channel = child->getIntAttribute("channel");
                    }
                }
            }
        }
    }
}
#endif

//==============================================================================
void FTMSynthAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    auto state = tree.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FTMSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(tree.state.getType()))
            tree.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FTMSynthAudioProcessor();
}
