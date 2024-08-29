/*
  ==============================================================================

    SynthVoice.h
    Created: 10 Oct 2018 3:46:36pm
    Authors: Lily H, Loïc J

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"

#define MAX_M1  16
#define MAX_M2  16
#define MAX_M3  16
#define SIN_LUT_MODULO     65535
#define SIN_LUT_RESOLUTION 65536

enum Algorithm {
    ivan, rabenstein
};


class SynthVoice : public SynthesiserVoice
{
public:
    bool canPlaySound(SynthesiserSound* sound);

    //==================================
    static void computeSinLUT();

    void getcusParam(std::atomic<float>* algo,
                     std::atomic<float>* volume,
                     std::atomic<float>* pitch,
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

    // ===== Methods used for the Ivan method
    void ivan_deff();
    void ivan_getf();
    void ivan_getSigma(double _tau, double p);
    void ivan_getw(double p);
    void ivan_getK();

    // ===== Methods used for the Rabenstein method
    void rabenstein_getCoefficients(double _tau, double _p);
    void rabenstein_getOmega();
    void rabenstein_getK();

    // ===== Common methods
    void findmax();
    void initDecayampn();
    double ivan_finaloutput();
    double rabenstein_finaloutput();


    //==================================
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int
                   currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;

    bool isVoiceActive() const override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void aftertouchChanged(int newAftertouchValue) override;
    void channelPressureChanged(int newChannelPressureValue) override;

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    void renderNextBlock(AudioBuffer<double> &outputBuffer, int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;
    double getSampleRate() const;
    bool isPlayingButReleased() const;
    bool wasStartedBefore(const SynthesiserVoice& other) const;


private:
    inline static double sinLUT[SIN_LUT_RESOLUTION];

    Algorithm currentAlgorithm, nextAlgorithm;
    double mainVolume;

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
    double sr = 44100;

    // FTM model parameters
    double fomega;            // frequency
    double ftau, frel;        // sustain
    bool bgate;
    double fp, fring;         // damping
    bool bpGate;
    double fd  = 0,   nextd;  // inharmonicity
    double fa  = 0.5, nexta;  // squareness
    double fa2 = 0.5, nexta2; // cubeness

    double r1, r2, r3;    // coordinates

    int m1 = 5; // shouldn't be bigger than MAX_M1
    int m2 = 5; // shouldn't be bigger than MAX_M2
    int m3 = 5; // shouldn't be bigger than MAX_M3
    int nextm1 = 5; // shouldn't be bigger than MAX_M1
    int nextm2 = 5; // shouldn't be bigger than MAX_M2
    int nextm3 = 5; // shouldn't be bigger than MAX_M3

    int dim, nextDim;


    // ===== Variables used for the Ivan method
    int tau = 300;

    double fx1[301]; // tau
    double fx2[301];
    double fx3[301];
    double f1[MAX_M1];
    double f2[MAX_M2];
    double f3[MAX_M3];

    // mode decay/damping factors
    double sigma1d[MAX_M1];
    double sigma2d[MAX_M1*MAX_M2];
    double sigma3d[MAX_M1*MAX_M2*MAX_M3];


    // ===== Variables used for the Rabenstein method
    double alpha1d[MAX_M1];
    double alpha2d[MAX_M1*MAX_M2];
    double alpha3d[MAX_M1*MAX_M2*MAX_M3];
    double beta1d[MAX_M1];
    double beta2d[MAX_M1*MAX_M2];
    double beta3d[MAX_M1*MAX_M2*MAX_M3];

    bool mode_rejected1d[MAX_M1];
    bool mode_rejected2d[MAX_M1*MAX_M2];
    bool mode_rejected3d[MAX_M1*MAX_M2*MAX_M3];

    double fN1d;
    double fN2d;
    double fN3d;

    double yi1d[MAX_M1];
    double yi2d[MAX_M1*MAX_M2];
    double yi3d[MAX_M1*MAX_M2*MAX_M3];


    // ===== Common variables
    // mode magnitudes
    double k1d[MAX_M1];
    double k2d[MAX_M1*MAX_M2];
    double k3d[MAX_M1*MAX_M2*MAX_M3];

    // mode frequencies
    double omega1d[MAX_M1];
    double omega2d[MAX_M1*MAX_M2];
    double omega3d[MAX_M1*MAX_M2*MAX_M3];

    // mode decay factors, sample-rate dependant
    double decayamp1[MAX_M1];
    double decayampn1[MAX_M1];
    double decayamp2[MAX_M1*MAX_M2];
    double decayampn2[MAX_M1*MAX_M2];
    double decayamp3[MAX_M1*MAX_M2*MAX_M3];
    double decayampn3[MAX_M1*MAX_M2*MAX_M3];

    double maxh = 1; // the max of h for each set of parameters
};
