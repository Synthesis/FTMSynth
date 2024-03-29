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
#include <algorithm>
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
                             std::atomic<float>* /*gate*/,
                             std::atomic<float>* /*rel*/,
                             std::atomic<float>* p,
                             std::atomic<float>* /*pGate*/,
                             std::atomic<float>* /*ring*/,
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
    fp = p->load();
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


void SynthVoice::initialize()
{
    float l0 = M_PI; // constant

    if (dim == 0)
    {
        float S = pow(pow(fd*fomega, 2) + pow(fp/ftau, 2), 0.25);
        float T = ((1 - fp*fp) / ftau*ftau + fomega*fomega * (1 - fd*fd));

        float d1 = 2 * (1 - fp) / ftau;
        float d3 = -2 * fp / ftau;

        float EI = pow(S, 4);

        for (int i=0; i<m1; i++)
        {
            n1d[i] = pow(i+1, 2);
            n1d2[i] = n1d[i]*n1d[i];

            k1d[i] = sin((i+1)*M_PI*r1);

            beta1d[i] = EI * n1d2[i] + T * n1d[i];
            alpha1d[i] = (d1 - d3 * n1d[i]) / 2;
            omega1d[i] = sqrt(abs(beta1d[i] - alpha1d[i]*alpha1d[i]));

            mode_rejected1d[i] = ((omega1d[i] / (2*M_PI)) > (sr / 2));
        }

        fN1d = M_PI / 4;

        for (int i=0; i<m1; i++)
        {
            yi1d[i] = level * sin((i+1)*M_PI*r1) / omega1d[i];
        }
    }
    else if (dim == 1)
    {
        float l2 = M_PI*fa;
        float beta = fa + 1/fa;

        float S = pow(pow(fd*fomega*fa, 2) + pow(fp*fa/ftau, 2), 0.25);
        float T = (fa * (1/beta - fp*fp*beta) / ftau*ftau
                + fa * fomega*fomega * (1/beta - fd*fd * beta));

        float d1 = 2 * (1 - fp*beta) / ftau;
        float d3 = -2 * fp * fa / ftau;

        float EI = pow(S, 4);

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                n2d[i+m1*j] = pow((i+1)*M_PI/l0, 2) + pow((j+1)*M_PI/l2, 2);
                n2d2[i+m1*j] = n2d[i+m1*j]*n2d[i+m1*j];

                k2d[i+m1*j] = sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2);

                beta2d[i+m1*j] = EI * n2d2[i+m1*j] + T * n2d[i+m1*j];
                alpha2d[i+m1*j] = (d1 - d3 * n2d[i+m1*j]) / 2;
                omega2d[i+m1*j] = sqrt(abs(beta2d[i+m1*j] - alpha2d[i+m1*j]*alpha2d[i+m1*j]));

                mode_rejected2d[i+m1*j] = ((omega2d[i+m1*j] / (2*M_PI)) > (sr / 2));
            }
        }

        fN2d = M_PI * l2 / 4;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                yi2d[i+m1*j] = level * sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2) / omega2d[i+m1*j];
            }
        }
    }
    else if (dim == 2)
    {
        float l2 = M_PI*fa;
        float l3 = M_PI*fa2;
        float beta = fa*fa2 + fa/fa2 + fa2/fa;

        float S = pow(pow(fd*fomega*fa, 2) + pow(fp*fa/ftau, 2), 0.25);
        float T = (fa*fa2 * (1/beta - fp*fp*beta) / ftau*ftau
                + fa*fa2 * fomega*fomega * (1/beta - fd*fd * beta));

        float d1 = 2 * (1 - fp*beta) / ftau;
        float d3 = -2 * fp * fa*fa2 / ftau;

        float EI = pow(S, 4);

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    n3d[i+m1*(j+m2*k)] = pow((i+1)*M_PI/l0, 2) + pow((j+1)*M_PI/l2, 2) + pow((k+1)*M_PI/l3, 2);
                    n3d2[i+m1*(j+m2*k)] = n3d[i+m1*(j+m2*k)]*n3d[i+m1*(j+m2*k)];

                    k3d[i+m1*(j+m2*k)] = sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2) * sin((k+1)*M_PI*r3);

                    beta3d[i+m1*(j+m2*k)] = EI * n3d2[i+m1*(j+m2*k)] + T * n3d[i+m1*(j+m2*k)];
                    alpha3d[i+m1*(j+m2*k)] = (d1 - d3 * n3d[i+m1*(j+m2*k)]) / 2;
                    omega3d[i+m1*(j+m2*k)] = sqrt(abs(beta3d[i+m1*(j+m2*k)] - alpha3d[i+m1*(j+m2*k)]*alpha3d[i+m1*(j+m2*k)]));

                    mode_rejected3d[i+m1*(j+m2*k)] = ((omega3d[i+m1*(j+m2*k)] / (2*M_PI)) > (sr / 2));
                }
            }
        }

        fN3d = M_PI * l2 * l3 / 8;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    yi3d[i+m1*(j+m2*k)] = level * sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2) * sin((k+1)*M_PI*r3) / omega3d[i+m1*(j+m2*k)];
                }
            }
        }
    }
}


// findmax functions find value of first sample and scale everything else based on this value
void SynthVoice::findmax()
{
    float h = 0;

    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            h += (yi1d[i]/level) * k1d[i] * exp(-alpha1d[i]*M_PI_2 / omega1d[i]);
        }
        h /= fN1d;
    }
    // 2D
    else if (dim == 1)
    {
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                h += (yi2d[i+m1*j]/level) * k2d[i+m1*j] * exp(-alpha2d[i+m1*j]*M_PI_2 / omega2d[i+m1*j]);
            }
        }
        h /= fN2d;
    }
    // 3D
    else if (dim == 2)
    {
        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    h += (yi3d[i+m1*(j+m2*k)]/level) * k3d[i+m1*(j+m2*k)] * exp(-alpha3d[i+m1*(j+m2*k)]*M_PI_2 / omega3d[i+m1*(j+m2*k)]);
                }
            }
        }
        h /= fN3d;
    }
    //
    else
    {
        h = 1;
    }

    maxh = h;
}


// this function synthesizes the signal value at each sample
double SynthVoice::finaloutput(int sample)
{
    if (trig == false) return 0;

    float h = 0;
    float lutScale = SIN_LUT_RESOLUTION / (2.0*M_PI);

    // synthesize the sound at time t
    if (dim == 0)
    {
        float decayamp;

        for (int i=0; i<m1; i++)
        {
            if (!mode_rejected1d[i])
            {
                decayamp = exp(-alpha1d[i]*t) * fastSin(omega1d[i]*t);
                h += (yi1d[i] * decayamp) * k1d[i];
            }
        }
        h /= fN1d;
    }
    else if (dim == 1)
    {
        float decayamp;
        int index = 0;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;
                if (!mode_rejected2d[index])
                {
                    decayamp = exp(-alpha2d[index]*t) * fastSin(omega2d[index]*t);
                    h += (yi2d[index] * decayamp) * k2d[index];
                }
            }
        }
        h /= fN2d;
    }
    else if (dim == 2)
    {
        float decayamp;
        int index = 0;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    index = i+m1*(j+m2*k);
                    if (!mode_rejected3d[index])
                    {
                        decayamp = exp(-alpha3d[index]*t) * fastSin(omega3d[index]*t);
                        h += (yi3d[index] * decayamp) * k3d[index];
                    }
                }
            }
        }
        h /= fN3d;
    }

    double output = h / (maxh*2); // maxh*2 to leave some headroom

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
    float ratio = 1.0f;
    if (bkbTrack)
    {
        // map keyboard to frequency
        float frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440);
        fomega = frequency * 2 * M_PI * pow(2.0, fpitch/12.0);
    }
    else
    {
        fomega = 440 * 2 * M_PI * pow(2.0, (fpitch - 9.0)/12.0);
    }
    pitchBend = (currentPitchWheelPosition - 8192) / 8192.0;

    // sound duration depending on sustain, tau = 0.075 means a 1-second output
    dur = log(1-ftau) / log(1-0.075);

    fd = nextd;

    m1 = nextm1;
    m2 = nextm2;
    m3 = nextm3;

    initialize();
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
    initialize();
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
