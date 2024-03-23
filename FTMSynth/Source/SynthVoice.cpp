/*
  ==============================================================================

    SynthVoice.cpp
    Created: 10 Oct 2018 3:46:36pm
    Authors: Lily H, Loïc J

  ==============================================================================
*/

#include "SynthVoice.h"

#include <math.h>
#include <cmath>
#include <array>
#include <complex>
#include <typeinfo>
#include <iomanip>

using namespace std;


bool SynthVoice::canPlaySound(SynthesiserSound* sound)
{
    // if succesfully cast sound into my own class, return true
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::computeSinLUT()
{
    for (int i = 0; i < SIN_LUT_RESOLUTION; i++)
    {
        sinLUT[i] = sin(i * 2.0 * M_PI / SIN_LUT_RESOLUTION);
    }
}

#define fastSin(x) sinLUT[int(x*lutScale) & SIN_LUT_MODULO]


//==================================
// some function that grabs value from the slider, and then either returns or set the signal of my synthesized drum sound
// IMPORTANT NOTE: the parameters need to be passed as std::atomic<float>*, from tree.getRawParameterValue("name"),
//                 otherwise they'll be applied AFTER the next note press, instead of before, which means the
//                 parameters will be updated one hit too late, which is what we *don't* want.
void SynthVoice::getcusParam(std::atomic<float>* pitch,
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
                             std::atomic<float>* dimensions)
{
    // this function fetches parameters from the customized GUI and calculates the
    // corresponding parameters in order to synthesize the sound
    fpitch = pitch->load();
    bkbTrack = (kbTrack->load() >= 0.5f);
    ftau = _tau->load();
    bgate = (gate->load() >= 0.5f);
    frel = rel->load();
    fp = p->load();
    bpGate = (pGate->load() >= 0.5f);
    fring = ring->load();
    nextd = dispersion->load();
    fa = alpha1->load();
    fa2 = alpha2->load();

    r1 = x->load();
    r2 = y->load();
    r3 = z->load();

    nextm1 = int(modesX->load());
    nextm2 = int(modesY->load());
    nextm3 = int(modesZ->load());

    nextDim = int(dimensions->load()) - 1;
}


void SynthVoice::getParams(float _tau, float p)
{
    float l0 = M_PI; // constant
    float l2 = l0*fa;

    float beta = fa + 1/fa;

    float S = l0/M_PI * pow(pow(fd*fomega*fa, 2) + pow(p*fa/_tau, 2), 0.25);

    float T = (
        fa * (1/beta - p*p*beta) / _tau*_tau
        + fa * fomega*fomega * (1/beta - fd*fd * beta)
    ) * pow(l0/M_PI, 2);

    float d1 = 2 * (1 - p*beta) / _tau;
    float d3 = -2 * p * fa / _tau * pow(l0/M_PI, 2);

    float EI = pow(S, 4);

    for (int i=0; i<min(m1, m2); i++)
    {
        n2d[i] = pow((i+1)*M_PI/l0, 2) + pow((i+1)*M_PI/l2, 2);
        n2d2[i] = n2d[i]*n2d[i];

        k2d[i] = sin((i+1)*M_PI*r1) * sin((i+1)*M_PI*r2);

        beta2d[i] = EI * n2d2[i] + T * n2d[i];
        alpha2d[i] = (d1 - d3 * n2d[i]) / 2;
        omega2d[i] = sqrt(abs(beta2d[i] - alpha2d[i]*alpha2d[i]));
    }

    modeCorr = 0;
    for (int i=0; i<min(m1, m2); i++)
    {
        if (omega2d[i]/2/M_PI <= sr/2) modeCorr++;
    }

    fn2d = l0 * l2 / 4;

    for (int i=0; i<modeCorr; i++)
    {
        yi2d[i] = level * sin((i+1)*M_PI*r1) * sin((i+1)*M_PI*r2) / omega2d[i];
    }
}


// findmax functions find value of first sample and scale everything else based on this value
void SynthVoice::findmax()
{
    maxh = (1688.6741/fomega)/fa/2000;
}


// this function synthesizes the signal value at each sample
double SynthVoice::finaloutput(int sample)
{
    if (trig == false) return 0;

    float h = 0;
    float lutScale = SIN_LUT_RESOLUTION / (2.0*M_PI);

    for (int i=0; i<modeCorr; i++)
    {
        // synthesize the sound at time t
        decayampn2[i] = exp(-alpha2d[i]*t) * sin(omega2d[i]*t);
        h +=  (yi2d[i] * decayampn2[i]) * k2d[i] / fn2d;
    }

    double output = h / (maxh * 2); // maxh*2 to leave some headroom

    nsamp += pow(2, pitchBend);
    t = nsamp/sr; // t advancing one sample

    if (t >= dur)
    {
        trig = false;
        clearCurrentNote();
    }

    return output;
}


//==================================
//==================================
void SynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int
                           currentPitchWheelPosition)
{
    dim = nextDim;

    level = velocity;
    // TODO Keyboard tracking and pitch calculations go here
    float ratio = 1.0f;
    if (bkbTrack)
    {
        // map keyboard to frequency
        float frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440);
        fomega = frequency * 8 * pow(2.0, (fpitch - 3.716214)/12.0);
    }
    else
    {
        fomega = 440 * 8 * pow(2.0, (fpitch - 12.716214)/12.0);
    }
    pitchBend = (currentPitchWheelPosition - 8192) / 8192.0;

    dur = 20.0*ftau; // computation duration depending on sustain

    fd = nextd;

    m1 = nextm1;
    m2 = nextm2;
    m3 = nextm3;

    getParams(ftau, fp);
    findmax();

    t = 0;
    nsamp = 0;
    trig = true;
    setKeyDown(true);
}

//==================================
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    if (!allowTailOff)
    {
        trig = false;
        dur = 0;
        clearCurrentNote();
    }
    else
    {
        if (bgate)
        {
            float elapsed = t/dur; // percentage of the elapsed duration
            float remaining = 1.0 - elapsed; // percentage of the remaining duration
            float newDur = 20.0*frel;
            dur = dur*elapsed + newDur*remaining;
        }
        if (bgate || bpGate)
        {
            float _tau = (bgate ? frel : ftau);
            float p = (bpGate ? fring : fp);
            getParams(_tau, p);
        }
        setKeyDown(false);
    }
}

//==================================
bool SynthVoice::isVoiceActive() const
{
    return trig;
}

//==================================
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    pitchBend = (newPitchWheelValue-8192)/8192.0;
}
//==================================
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}
//==================================
void SynthVoice::aftertouchChanged(int newAftertouchValue)
{
}
//==================================
void SynthVoice::channelPressureChanged(int newChannelPressureValue)
{
}
//==================================
void SynthVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    // callback function
    for (int sample=0; sample<numSamples; ++sample)
    {
        // put the synthesized drum sound here
        double drumSound = finaloutput(sample);

        for (int channel=0; channel<outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample+sample, drumSound);
        }
    }
}

//==================================
void SynthVoice::renderNextBlock(AudioBuffer<double> &outputBuffer, int startSample, int numSamples)
{
}

//==================================
void SynthVoice::setCurrentPlaybackSampleRate(double newRate)
{
    sr = newRate;
    float p = (((!bpGate) || isKeyDown()) ? fp : fring);
    getParams(ftau, p);
}
//==================================
double SynthVoice::getSampleRate() const
{
    return sr;
}

//==================================
bool SynthVoice::isPlayingButReleased() const
{
    return (trig && !isKeyDown());
}
//==================================
bool SynthVoice::wasStartedBefore(const SynthesiserVoice& other) const
{
    SynthesiserVoice& otherRef = const_cast<SynthesiserVoice&>(other);
    try
    {
        SynthVoice& otherVoice = dynamic_cast<SynthVoice&>(otherRef);

        // equal sign is very important for voice removal (to determine whether the current voice is the oldest)
        return (t >= otherVoice.t);
    }
    catch (bad_cast e)
    {
        return false;
    }
}
