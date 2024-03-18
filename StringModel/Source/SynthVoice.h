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

#define MAX_M1  16
#define MAX_M2  16
#define MAX_M3  16
#define SIN_LUT_MODULO     65535
#define SIN_LUT_RESOLUTION 65536


class SynthVoice: public SynthesiserVoice
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


    void deff();
    void getf();
    void getSigma(float _tau, float p);
    void getw(float p);
    void getK();

    void findmax();

    double finaloutput(int sample);

    //==================================
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int
                    currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;

    bool isVoiceActive() const override;

    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void aftertouchChanged (int newAftertouchValue) override;
    void channelPressureChanged (int newChannelPressureValue) override;

    void renderNextBlock (AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
    void renderNextBlock (AudioBuffer< double > &outputBuffer, int startSample, int numSamples);

    void setCurrentPlaybackSampleRate (double newRate) override;
    double getSampleRate() const;
    bool isPlayingButReleased() const;
    bool wasStartedBefore(const SynthesiserVoice& other) const;


private:
    double sr = 44100;

    // note parameters
    double level;
    double frequency;
    bool bkbTrack;
    double fpitch; // in semitones
    double pitchBend; // in octaves

    // time-related variables
    bool trig;
    double t;
    double nsamp;
    double dur; // in seconds

    // drum model parameters
    float ftau;
    float fomega;
    float fp;
    float fd = 0;
    float nextd;
    float fa;
    float fa2;
    float r1;
    float r2;
    float r3;
    int dim;
    int nextDim;
    bool bgate;
    float frel;
    bool bpGate;
    float fring;

    int tau = 300;

    float fx1[301]; // tau
    float fx2[301];
    float fx3[301];
    float f1[MAX_M1];
    float f2[MAX_M2];
    float f3[MAX_M3];

    int m1 = 5; // shouldn't be bigger than MAX_M1
    int m2 = 5; // shouldn't be bigger than MAX_M2
    int m3 = 5; // shouldn't be bigger than MAX_M3
    int nextm1 = 5; // shouldn't be bigger than MAX_M1
    int nextm2 = 5; // shouldn't be bigger than MAX_M2
    int nextm3 = 5; // shouldn't be bigger than MAX_M3

    // mode decay/damping factors
    float sigma1d[MAX_M1];
    float sigma2d[MAX_M1*MAX_M2];
    float sigma3d[MAX_M1*MAX_M2*MAX_M3];

    // and their sample-rate-dependant counterparts
    float decayamp1[MAX_M1];
    float decayampn1[MAX_M1];
    float decayamp2[MAX_M1*MAX_M2];
    float decayampn2[MAX_M1*MAX_M2];
    float decayamp3[MAX_M1*MAX_M2*MAX_M3];
    float decayampn3[MAX_M1*MAX_M2*MAX_M3];

    // mode frequencies
    float omega1d[MAX_M1];
    float omega2d[MAX_M1*MAX_M2];
    float omega3d[MAX_M1*MAX_M2*MAX_M3];

    // mode magnitudes
    float k1d[MAX_M1];
    float k2d[MAX_M1*MAX_M2];
    float k3d[MAX_M1*MAX_M2*MAX_M3];

    float maxh = 1; //the max of h for each set of parameters

    inline static double sinLUT[SIN_LUT_RESOLUTION];
};
