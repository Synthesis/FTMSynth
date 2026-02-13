/*
  ==============================================================================

    PluginProcessor.h
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

#pragma once

#include <map>
#include <JuceHeader.h>
#include "SynthSound.h"
#include "SynthVoice.h"

//==============================================================================
struct MidiMappingEntry
{
    std::atomic<int> cc { -1 };       // -1 = OFF
    std::atomic<int> channel { -2 };  // -2 = MAIN, -1 = OMNI, 0-15 = channels 1-16
};

//==============================================================================
struct ParamInfo
{
    const char* paramID;
    const char* displayName;
};

static constexpr ParamInfo paramTable[] = {
    { "volume",     "VOLUME" },
    { "attack",     "ATTACK" },
    { "dimensions", "DIMENSIONS" },
    { "pitch",      "PITCH" },
    { "kbTrack",    "KB TRACK" },
    { "sustain",    "SUSTAIN" },
    { "susGate",    "SUSTAIN GATE" },
    { "release",    "RELEASE" },
    { "damp",       "DAMP" },
    { "dampGate",   "DAMP GATE" },
    { "ring",       "RING" },
    { "dispersion", "INHARMONICITY" },
    { "squareness", "SQUARENESS" },
    { "cubeness",   "CUBENESS" },
    { "r1",         "IMPULSE X" },
    { "r2",         "IMPULSE Y" },
    { "r3",         "IMPULSE Z" },
    { "m1",         "MODES X" },
    { "m2",         "MODES Y" },
    { "m3",         "MODES Z" },
    { "voices",     "POLY VOICES" },
    { "algorithm",  "ALGORITHM" },
};

static constexpr int numMappableParams = (int)std::size(paramTable);

//==============================================================================
class FTMSynthAudioProcessor : public AudioProcessor, public ChangeBroadcaster, public AsyncUpdater
{
public:
    //==============================================================================
    FTMSynthAudioProcessor();
    ~FTMSynthAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    // MIDI Mapping
    std::map<String, std::unique_ptr<MidiMappingEntry>> midiMappings;
    std::atomic<int> defaultChannel { -1 };  // -1 = OMNI, 0-15 = channels 1-16
    void setMidiMapping(const String& paramID, int cc, int channel);

    // Learn Mode
    std::atomic<bool> learningCC { false };
    std::atomic<bool> learningChannel { false };
    String learningParamID;
    void setMidiLearn(const String& paramID, bool learnCC, bool learnChannel);
    void handleAsyncUpdate() override;

    // Persistence
    static PropertiesFile::Options getGlobalSettingsOptions();
    void saveGlobalMidiMappings();
    void loadGlobalMidiMappings();

    std::unique_ptr<XmlElement> getMidiMappingsAsXml();
    void restoreMidiMappingsFromXml(const XmlElement& xml);

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void resetAllParametersToDefault();

    //==============================================================================
    AudioProcessorValueTreeState tree;  // to link values from the slider to processor

private:
    Synthesiser mySynth;

    double lastSampleRate;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FTMSynthAudioProcessor)
};
