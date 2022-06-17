/*
  ==============================================================================
 
    SynthesiserVoice.h
    Created: 10 Oct 2018 3:46:36pm
    Author:  Lily H
 
  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "./SynthSound.h"

#include <math.h>       /* pow */
#include <cmath>
#include <algorithm>
#include <array>
#include <complex>
#include <typeinfo>
#include <iomanip>

#define MAX_M1  5
#define MAX_M2  5
#define MAX_M3  5

using namespace std;


class SynthVoice: public SynthesiserVoice
{
public:
    bool canPlaySound(SynthesiserSound* sound)
    {
        // if succesfully cast sound into my own class, return true
        return dynamic_cast<SynthSound*>(sound)!=nullptr;
    }

    //==================================
    // some function that grabs value from the slider, and then either returns or set the signal of my synthesized drum sound
    void getcusParam(std::atomic<float>* _tau,
                     std::atomic<float>* p,
                     std::atomic<float>* dispersion,
                     std::atomic<float>* alpha1,
                     std::atomic<float>* alpha2,
                     std::atomic<float>* x,
                     std::atomic<float>* y,
                     std::atomic<float>* z,
                     std::atomic<float>* dimensions)
    {
        // this function fetch parameters from the customized GUI and calculate the corresponding parameters in order to synthesize the sound
        // for each dimension, different algorithms are called
        ftau = _tau->load();
        fp = p->load();
        fd = dispersion->load();
        fa = alpha1->load();
        fa2 = alpha2->load();

        r1 = x->load();
        r2 = y->load();
        r3 = z->load();

        nextDim = int(dimensions->load()) - 1;
    }


    // define f(x) as a gaussian distribution with mean at the middle point l/2
    void deff()
    {
        float s=0.4; // standard deviation
        if (dim >= 0)
        {
            float h = M_PI/tau;
            for(int i=0; i < tau+1; i++)
            {
                fx1[i]=( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (i*h-M_PI*r1)/s, 2.0 ) );
            }
        }
        if (dim >= 1)
        {
            float h = fa*M_PI/tau;
            for(int i=0; i < tau+1; i++)
            {
                fx2[i]=( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (i*h-fa*M_PI*r2)/s, 2.0 ) );
            }
        }
        if (dim >= 2)
        {
            float h = fa2*M_PI/tau;
            for(int i=0; i < tau+1; i++)
            {
                fx3[i]=( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (i*h-fa2*M_PI*r3)/s, 2.0 ) );
            }
        }
    }

    // get coefficients of the integral f1m1 using trapezoid rule
    void getf()
    {
        // integrate f(x)sin(mpix/l)dx from 0 to l using trapezoid rule
        float integral;

        if (dim >= 0)
        {
            float l=M_PI;
            int m=m1;
            float h=l/tau;
            for(int j=0;j<m;j++)
            {
                integral=0;
                for(int i=0;i<tau;i++)
                {
                    integral+=(fx1[i+1]*sin((i+1)*h*M_PI*(j+1)/l)+fx1[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;//(f(b)+f(a))*(b-a)/2
                }
                f1[j]=2*integral/l;
            }
        }
        if (dim >= 1)
        {
            float l=fa*M_PI;
            int m=m2;
            float h=l/tau;
            for(int j=0;j<m;j++)
            {
                integral=0;
                for(int i=0;i<tau;i++)
                {
                    integral+=(fx2[i+1]*sin((i+1)*h*M_PI*(j+1)/l)+fx2[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;//(f(b)+f(a))*(b-a)/2
                }
                f2[j]=2*integral/l;
            }
        }
        if (dim >= 2)
        {
            float l=fa2*M_PI;
            int m=m3;
            float h=l/tau;
            for(int j=0;j<m;j++)
            {
                integral=0;
                for(int i=0;i<tau;i++)
                {
                    integral+=(fx3[i+1]*sin((i+1)*h*M_PI*(j+1)/l)+fx2[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;//(f(b)+f(a))*(b-a)/2
                }
                f3[j]=2*integral/l;
            }
        }
    }


    // intermediate variables
    // sigma
    void getSigma()
    {
        float sigma1=-1/ftau;

        // 1D
        if (dim == 0)
        {
            for(int i=0;i<m1;i++)
            {
                sigma1d[i]=sigma1*(1+fp*(pow(i+1,2)-1));
                decayamp1[i]=exp(sigma1d[i]/sr);
            }
        }
        // 2D
        else if (dim == 1)
        {
            float beta=fa+1/fa;
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    sigma2d[i*m1+j]=sigma1*(1+fp*(pow(i+1,2)*fa+pow(j+1,2)/fa-beta));
                    decayamp2[i*m1+j]=exp(sigma2d[i*m1+j]/sr);
                }
            }
        }
        // 3D
        else if (dim == 2)
        {
            float beta=fa*fa2+fa/fa2+fa2/fa;
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    for(int m=0;m<m3;m++)
                    {
                        sigma3d[(i*m1+j)*m2+m]=(sigma1*(1+fp*(pow(i+1,2)*fa*fa2+pow(j+1,2)*fa2/fa+pow(m+1,2)*fa/fa2-beta)));
                        decayamp3[(i*m1+j)*m2+m]=exp(sigma3d[(i*m1+j)*m2+m]/sr);
                    }
                }
            }
        }
    }


    // get coefficient omega for the impulse response
    void getw()
    {
        // 1D
        if (dim == 0)
        {
            float sigma=-1/ftau;
            for(int i=0;i<m1;i++)
            {
                float interm=pow(i+1,2);//M^2
                omega1d[i]= sqrt(pow(fd*fomega*interm,2)+interm*(pow(sigma*(1-fp),2)+pow(fomega,2)*(1-pow(fd,2)))-pow(sigma*(1-fp),2));
            }
        }
        // 2D
        else if (dim == 1)
        {
            float beta=fa+1/fa;
            float sigma=-1/ftau;
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    float interm=pow(i+1,2)*fa+pow(j+1,2)/fa;
                    omega2d[i*m1+j]= sqrt(pow(fd*fomega*interm,2)+interm*(pow(sigma*(1-fp*beta),2)/beta+pow(fomega,2)*(1-pow(fd*beta,2))/beta)-pow(sigma*(1-fp*beta),2));
                }
            }
        }
        // 3D
        else if (dim == 2)
        {
            float beta=fa*fa2+fa/fa2+fa2/fa;
            float sigma=-1/ftau;
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    for(int m=0;m<m3;m++)
                    {
                        float interm=pow(i+1,2)*fa*fa2+pow(j+1,2)*fa2/fa+pow(m+1,2)*fa/fa2;
                        omega3d[(i*m1+j)*m2+m]= sqrt(pow(fd*fomega*interm,2)+interm*(pow(sigma*(1-fp*beta),2)/beta+pow(fomega,2)*(1-pow(fd*beta,2))/beta)-pow(sigma*(1-fp*beta),2));
                    }
                }
            }
        }
    }


    // get coefficient k for the impulse response
    void getK()
    {
        float l1=M_PI;
        float x1=l1*r1;

        // 1D
        if (dim == 0)
        {
            for(int i=0;i<m1;i++)
            {
                k1d[i]=f1[i]*sin((i+1)*x1*M_PI/l1)/omega1d[i];
            }
        }
        // 2D
        else if (dim == 1)
        {
            float l2=fa*M_PI;
            float x2=l2*r2;

            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    k2d[i*m1+j]=f1[i]*f2[j]*sin((i+1)*x1*M_PI/l1)*sin((j+1)*x2*M_PI/l2)/omega2d[i*m1+j];
                }
            }
        }
        // 3D
        else if (dim == 2)
        {
            float l2=fa*M_PI;
            float l3=fa2*M_PI;
            float x2=l2*r2;
            float x3=l3*r3;

            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    for(int m=0;m<m3;m++)
                    {
                        k3d[(i*m1+j)*m2+m]=f1[i]*f2[j]*f3[m]*sin((i+1)*x1*M_PI/l1)*sin((j+1)*x2*M_PI/l2)*sin((m+1)*x3*M_PI/l3)/omega3d[(i*m1+j)*m2+m];
                    }
                }
            }
        }
    }


    // findmax functions find value of first sample and scale everything else based on this value
    void findmax()
    {
        float h=0;

        // 1D
        if (dim == 0)
        {
            for(int i=0;i<m1;i++)
            {
                h += k1d[i]*exp(sigma1d[i]*M_PI/2/omega1d[i]);
            }
        }
        // 2D
        else if (dim == 1)
        {
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    h += k2d[i*m1+j]*exp(sigma2d[i*m1+j]*M_PI/2/omega2d[i*m1+j]);
                }
            }
        }
        // 3D
        else if (dim == 2)
        {
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    for(int m=0;m<m3;m++)
                    {
                        h += k3d[(i*m1+j)*m2+m]*exp(sigma3d[(i*m1+j)*m2+m]*M_PI/2/omega3d[(i*m1+j)*m2+m]);
                    }
                }
            }
        }

        maxh = h;
    }


    //==================================
    //==================================
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int
                    currentPitchWheelPosition)
    {
        // the keyboard-voice activation interface, should write different excitation signals here.
        dim = nextDim;

        level = velocity;
        // map keyboard to frequency
        frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440);
        //std::cout<<"midinote "<<midiNoteNumber<<"\n";
        fomega = frequency*8 * pow(2.0, -4.18/12.0);
        pitchBend = (currentPitchWheelPosition-8192)/8192.0;

        dur = 20.0*ftau; // computation duration depending on sustain

        deff();
        getf();

        getSigma();
        getw();
        getK();
        findmax();

        t = 0;
        nsamp = 0;
        trig = true;
        setKeyDown(true);
    }

    //==================================
    void stopNote (float velocity, bool allowTailOff)
    {
        if (!allowTailOff)
        {
            trig = false;
            clearCurrentNote();
        }
    }

    //==================================
    bool isVoiceActive()
    {
        return trig;
    }

    // this function synthesize signalvalue at each sample
    double finaloutput(int sample)
    {
        if (trig == false) return 0;

        float h=0;

        // 1D
        if (dim == 0)
        {
            for(int i=0;i<m1;i++)
            {
                if(nsamp==0){
                    decayampn1[i]=decayamp1[i];
                }
                else{
                    decayampn1[i]*=decayamp1[i];
                }
                
                h+=k1d[i]*decayampn1[i]*sin(omega1d[i]*t);
            }
        }
        // 2D
        else if (dim == 1)
        {
            // sum up to mode m1 and mode m2
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    // designate the exponential envelope
                    if(nsamp==0){
                        decayampn2[i*m1+j]=decayamp2[i*m1+j];
                    }
                    else{
                        decayampn2[i*m1+j]*=decayamp2[i*m1+j];
                    }
                    // synthesize the sound at time t
                    h+=k2d[i*m1+j]*decayampn2[i*m1+j]*sin(omega2d[i*m1+j]*t);
                }
            }
        }
        // 3D
        else if (dim == 2)
        {
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    for(int m=0;m<m3;m++)
                    {
                        if (nsamp==0) {
                            decayampn3[(i*m1+j)*m2+m]=decayamp3[(i*m1+j)*m2+m];
                        }
                        else {
                            decayampn3[(i*m1+j)*m2+m]*=decayamp3[(i*m1+j)*m2+m];
                        }
                        h+=k3d[(i*m1+j)*m2+m]*decayampn3[(i*m2+j)*m2+m]*sin(omega3d[(i*m1+j)*m2+m]*t);
                    }
                }
            }
        }

        double output = (h*level)/(maxh*2); // maxh*2 to leave some headroom

        nsamp += pow(2,pitchBend);
        t = nsamp/sr; // t advancing one sample

        if (t >= dur)
        {
            trig = false;
            clearCurrentNote();
        }

        return output;
    }

    //==================================
    void pitchWheelMoved (int newPitchWheelValue)
    {
        pitchBend = (newPitchWheelValue-8192)/8192.0;
    }
    //==================================
    void controllerMoved (int controllerNumber, int newControllerValue)
    {
    }
    //==================================
    void aftertouchChanged (int newAftertouchValue)
    {
    }
    //==================================
    void channelPressureChanged (int newChannelPressureValue)
    {
    }
    //==================================
    void renderNextBlock (AudioBuffer< float > &outputBuffer, int startSample, int numSamples)
    {
        // callback function

        for(int sample=0;sample<numSamples;++sample)
        {
            // put the synthesized drum sound here
            double drumSound = finaloutput(sample);

            for(int channel=0;channel<outputBuffer.getNumChannels();++channel)
            {
                outputBuffer.addSample(channel, startSample+sample, drumSound);
            }
        }
    }

    //==================================
    void renderNextBlock (AudioBuffer< double > &outputBuffer, int startSample, int numSamples)
    {
    }
    //==================================
    void setCurrentPlaybackSampleRate (double newRate)
    {
        sr = newRate;
        getSigma();
    }
    //==================================
    double getSampleRate()
    {
        return sr;
    }
    //==================================
    bool isPlayingButReleased()
    {
    }
    //==================================
    bool wasStartedBefore(const SynthesiserVoice& other)
    {
    }
    //==================================
    //==================================


private:
    double sr = 44100;

    // note parameters
    double level;
    double frequency;
    double pitchBend;

    // time-related variables
    bool trig;
    double t;
    double nsamp;
    double dur; // in seconds

    // drum model parameters
    float ftau;
    float fomega;
    float fp;
    float fd;
    float fa;
    float fa2;
    float r1;
    float r2;
    float r3;
    int dim;
    int nextDim;

    int tau=300;

    float fx1[301]; //tau
    float fx2[301];
    float fx3[301];
    float f1[MAX_M1];
    float f2[MAX_M2];
    float f3[MAX_M3];

    int m1 = MAX_M1; // can't be bigger than MAX_M1
    int m2 = MAX_M2; // can't be bigger than MAX_M2
    int m3 = MAX_M3; // can't be bigger than MAX_M3

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
};
