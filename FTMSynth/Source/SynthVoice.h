/*
  ==============================================================================

    SynthVoice.h
    Created: 10 Oct 2018 3:46:36pm
    Authors: Lily H, Loïc J

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "./SynthSound.h"
#include <algorithm>

#define MAX_M1  16
#define MAX_M2  16
#define MAX_M3  16
#define SIN_LUT_MODULO     65535
#define SIN_LUT_RESOLUTION 65536

using namespace std;


class SynthVoice : public SynthesiserVoice
{
public:
    bool canPlaySound(SynthesiserSound* sound);

    static void computeSinLUT();

    //==================================
    void getcusParam(std::atomic<float>* pitch,
                     std::atomic<float>* kbTrack,
                     std::atomic<float>* _tau,
                     std::atomic<float>* gate,
                     std::atomic<float>* rel,
                     std::atomic<float>* p,
                     std::atomic<float>* pGate,
                     std::atomic<float>* ring,
                     std::atomic<float>* dispersion,
                     std::atomic<float>* alpha1,
                     std::atomic<float>* alpha2,
                     std::atomic<float>* x,
                     std::atomic<float>* y,
                     std::atomic<float>* z,
                     std::atomic<float>* modesX,
                     std::atomic<float>* modesY,
                     std::atomic<float>* modesZ,
                     std::atomic<float>* dimensions);

    void getParams(float _tau, float p);
    void findmax();

    double finaloutput(int sample);

    //==================================
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int
                   currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;

    bool isVoiceActive() const override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void aftertouchChanged(int newAftertouchValue) override;
    void channelPressureChanged(int newChannelPressureValue) override;

    void renderNextBlock(AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
    void renderNextBlock(AudioBuffer< double > &outputBuffer, int startSample, int numSamples);

    void setCurrentPlaybackSampleRate(double newRate) override;
    double getSampleRate() const;
    bool isPlayingButReleased() const;
    bool wasStartedBefore(const SynthesiserVoice& other) const;


private:
    double sr = 44100;

    // note parameters
    double level;
    bool bkbTrack;
    double fpitch; // in semitones (plugin's pitch knob)
    double pitchBend; // in octaves (pitch bend MIDI CC)

    // time-related variables
    bool trig;
    double t;
    double nsamp;
    double dur; // in seconds

    // FTM model parameters
    float fomega;        // frequency
    float ftau, frel;    // sustain
    bool bgate;
    float fp, fring;     // damping
    bool bpGate;
    float fd = 0, nextd; // inharmonicity
    float fa, fa2;       // squareness/cubeness

    float r1, r2, r3;    // coordinates

    int m1 = 5; // shouldn't be bigger than MAX_M1
    int m2 = 5; // shouldn't be bigger than MAX_M2
    int m3 = 5; // shouldn't be bigger than MAX_M3
    int nextm1 = 5; // shouldn't be bigger than MAX_M1
    int nextm2 = 5; // shouldn't be bigger than MAX_M2
    int nextm3 = 5; // shouldn't be bigger than MAX_M3

    int dim, nextDim;

    // ===== excitation algorithm parameters

    // ===== displacement & release algorithm parameters

    float n2d[max({MAX_M1, MAX_M2, MAX_M3})];
    float n2d2[max({MAX_M1, MAX_M2, MAX_M3})];
    float k2d[max({MAX_M1, MAX_M2, MAX_M3})];
    float alpha2d[max({MAX_M1, MAX_M2, MAX_M3})];
    float beta2d[max({MAX_M1, MAX_M2, MAX_M3})];
    float omega2d[max({MAX_M1, MAX_M2, MAX_M3})];
    float fn2d;
    int modeCorr;
    float yi2d[max({MAX_M1, MAX_M2, MAX_M3})];
    float decayampn2[max({MAX_M1, MAX_M2, MAX_M3})];

    float maxh = 1; // the max of h for each set of parameters

    inline static double sinLUT[SIN_LUT_RESOLUTION];
};
