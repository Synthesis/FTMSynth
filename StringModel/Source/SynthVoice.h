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
//#include "maximilian.h"
#include <math.h>       /* pow */
#include <algorithm>
#include<array>
//#include "./PluginProcessor.h"
#define MAX_M1  5
#define MAX_M2  5

class SynthVoice: public SynthesiserVoice
{
public:
    bool canPlaySound(SynthesiserSound* sound )
    {
        //if succesfully cast sound into my own class, return true
        return dynamic_cast<SynthSound*>(sound)!=nullptr;
    };
    
    //==================================
    //some function that grabs value from the slider, and then either returns or set the signal of my synethesized drum sound
    
    void setPosition(float a1,float a2)
    {
        r1 = a1;
        r2 = a2;
        //std::cout<<"elsewhere "<<r1<<" "<<r2<<"\n";
    }
    
    void getcusParam(std::atomic<float>* tau,
                     std::atomic<float>* omega,
                     std::atomic<float>* p,
                     std::atomic<float>* dispersion,
                     std::atomic<float>* alpha1,
                     std::atomic<float>* alpha2,
                     int dimtype,
                     std::atomic<float>* length,
                     std::atomic<float>* thickness)
    {
        //this function fetch parameters from the customized GUI and calculate the corresponding parameters in order to synthesize the sound
        //for each dimension, different algorithms are called
        //std::cout<<"which dimension "<<dim<<"\n";
        ftau=tau->load();
        fomega=omega->load();
        fp=p->load();
        fd=dispersion->load();
        fa=alpha1->load();
        fa2=alpha2->load();
        dim=dimtype;
        //if(1-1/fp<0){lengthscale = 2*M_PI;}
        //else{lengthscale = M_PI/sqrt(1-1/fp);}
        l1 = length->load(); //length is from 0.2 to 0.8, so it's setting l to be less than its max allowed value
        fthick = thickness->load();

        //std::cout<<"length "<<l1<<"\n";
        //2-D drum
        if(dim==1){
            //set the three vectors - sigma, omega, K to use later
            deff(1);
            deff(2);
            getf(1);
            getf(2);
            
            getsigma();
            getw();
            getK();
            findmax();
        }
        //1-D string
        if(dim==0){
            deff(1);
            getf(1);
            
            getsigma1d();
            getw1d();
            getK1d();
            findmax1d();
        }
        //3-D drum
        if(dim==2){
            deff(1);
            deff(2);
            deff(3);
            getf(1);
            getf(2);
            getf(3);
            getsigma3d();
            getw3d();
            getK3d();
            findmax3d();
        }
    }
    
    //findmax functions find value of first sample and scale everything else based on this value
    void findmax()
    {
        float h=0;
        float t1=M_PI/2/omega[0];
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                h+=k[i*m1+j]*exp(sigma[i*m1+j]*t1)*sin(omega[i*m1+j]*t1);
            }
            
        }
        maxh=h;
    }
    void findmax1d()
    {
        float h=0;
        float t1=M_PI/2/omega1d[0];
        for(int i=0;i<m1;i++)
        {
            
            h+=k1d[i]*exp(sigma1d[i]*t1)*sin(omega1d[i]*t1);
            
        }
        maxh=h;
    }
    void findmax3d()
    {
        float h=0;
        float t1=M_PI/2/omega3d[0];
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                for(int m=0;m<m3;m++)
                {
                    h+=k3d[i*m1+j*m2+m]*exp(sigma3d[i*m1+j*m2+m]*t1)*sin(omega3d[i*m1+j*m2+m]*t1);
                }
            }
        }
        maxh=h;
    }
    
    
    //this function synthesize signalvalue at each sample
    double finaloutput(int sample)
    {
        float h=0;
        //2-D
        if(dim==1)
        {
            //sum up to mode m1 and mode m2
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    //designate the exponential envelope
                    if(nsamp==0){
                        decayampn[i*m1+j]=decayamp[i*m1+j];
                    }
                    else{
                        decayampn[i*m1+j]*=decayamp[i*m1+j];
                    }
                    //synthesize the sound at time t
                    h+=k[i*m1+j]*decayampn[i*m1+j]*sin(omega[i*m1+j]*t);
                }
            }
        }
        //1-D
        else if(dim==0)
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
        //3-D
        else
        {
            for(int i=0;i<m1;i++)
            {
                for(int j=0;j<m2;j++)
                {
                    for(int m=0;m<m3;m++)
                    {
                        if(nsamp==0){
                            decayampn3[i*m1+j*m2+m]=decayamp3[i*m1+j*m2+m];
                        }
                        else{
                            decayampn3[i*m1+j*m2+m]*=decayamp3[i*m1+j*m2+m];
                        }
                        h+=k3d[i*m1+j*m2+m]*decayampn3[i*m2+j*m2+m]*sin(omega3d[i*m1+j*m2+m]*t);
                    }
                }
                
            }
        }
        //scale the output
        output=h/maxh/3;
        if(trig!=0)//if note is pressed, start counting time samples
        {
            t+=1/sr;//t advancing one sample
            nsamp+=1;
        }
        return double(output);
    }
    
    //intermediate variables
    //sigma
    void getsigma()
    {
        float sigma1=-1/ftau;
        float beta=fa+1/fa;
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                sigma[i*m1+j]=sigma1*(1+fp*(pow(i+1,2)*fa+pow(j+1,2)/fa-beta));
                decayamp[i*m1+j]=exp(sigma[i*m1+j]/sr);
            }
        }
    }
    
    void getsigma1d()
    {
        float sigma1=-1/ftau;
        for(int i=0;i<m1;i++)
        {
            sigma1d[i]=sigma1*(1+fp*(pow(i+1,2)-1));
            decayamp1[i]=exp(sigma1d[i]/sr);
        }
    }
    
    void getsigma3d()
    {
        float sigma1=-1/ftau;
        float beta=fa*fa2+fa/fa2+fa2/fa;
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                for(int m=0;m<m3;m++)
                {
                    sigma3d[i*m1+j*m2+m]=(sigma1*(1+fp*(pow(i+1,2)*fa*fa2+pow(j+1,2)*fa2/fa+pow(m+1,2)*fa/fa2-beta)));
                    decayamp3[i*m1+j*m2+m]=exp(sigma3d[i*m1+j*m2+m]/sr);
                }
            }
        }
    }
    
    //get coefficients of the integral f1m1 using trapezoid rule
    void getf(int which)
    {
        //integrate f(x)sin(mpix/l)dx from 0 to l using trapezoid rule
        float l;
        int m;
        float integral;
        //float l1=M_PI;
        float l2=fa*l1;
        float l3=fa2*l1;
        if(which==1) {l=l1;m=m1;}
        else if(which==2) {l=l2;m=m2;}
        else{l=l3;m=m3;}
        float h=l/tau;
        if(which==1)//first side
        {
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
        else if(which==2)//second side
        {
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
        else//third side
        {
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
    
    //define f(x) as a gaussian distribution with mean at a customized point l*r1
    void deff(int which)
    {
        float s=0.4;//standard deviation
        if(which==1)
        {
            float h=M_PI/tau;
            for(int i=0;i<tau+1;i++)
            {
                fx1[i]=( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (i*h-M_PI*r1)/s, 2.0 ) );
            }
           // std::cout<<"r1 "<<r1<<" "<<r2<<"\n";
        }
        else if(which==2)
        {
            float h=fa*M_PI/tau;
            for(int i=0;i<tau+1;i++)
            {
                fx2[i]=( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (i*h-fa*M_PI*r2)/s, 2.0 ) );
            }
        }
        else
        {
            float h=fa*M_PI/tau;
            for(int i=0;i<tau+1;i++)
            {
                fx3[i]=( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (i*h-fa2*M_PI/2.0)/s, 2.0 ) );
            }
        }
    }
    
    //get coefficient k for the impulse response
    void getK()
    {
        l1 = M_PI;
        float l2=fa*l1;
        float x1=l1*r1;
        float x2=l2*r2;
        //std::cout<<"in here?\n";
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                k[i*m1+j]=f1[i]*f2[j]*sin((i+1)*x1*M_PI/l1)*sin((j+1)*x2*M_PI/l2)/omega[i*m1+j];
            }
        }
        
    }
    void getK1d()
    {
        //float l1=M_PI;
        float x1=l1/2;
        
        
        for(int i=0;i<m1;i++)
        {
            
            k1d[i]=f1[i]*sin((i+1)*x1*M_PI/l1)/omega1d[i];
            
        }
        
    }
    void getK3d()
    {
        //float l1=M_PI;
        float l2=fa*l1;
        float l3=fa2*l1;
        float x1=l1/2;
        float x2=l2/2;
        float x3=l3/2;
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                for(int m=0;m<m3;m++)
                {
                    k3d[i*m1+j*m2+m]=f1[i]*f2[j]*f3[m]*sin((i+1)*x1*M_PI/l1)*sin((j+1)*x2*M_PI/l2)*sin((m+1)*x3*M_PI/l3)/omega3d[i*m1+j*m2+m];
                }
            }
        }
        
    }
    
    //get coefficient omega for the impulse response
    void getw()
    {
        float beta=fa+1/fa;
        float sigma=-1/ftau;
        for(int i=0;i<m1;i++)
        {
            for(int j=0;j<m2;j++)
            {
                float interm=pow(i+1,2)*fa+pow(j+1,2)/fa;
                
                omega[i*m1+j]= sqrt(pow(fd*fomega*interm,2)+interm*(pow(sigma*(1-fp*beta),2)/beta+pow(fomega,2)*(1-pow(fd*beta,2))/beta)-pow(sigma*(1-fp*beta),2));
                
            }
        }
    }
    void getw1d()
    {
        float sigma=-1/ftau;
        for(int i=0;i<m1;i++)
        {
            
            float interm=pow(i+1,2);//M^2
            omega1d[i]= sqrt(pow(fd*fomega*interm,2)+interm*(pow(sigma*(1-fp),2)+pow(fomega,2)*(1-pow(fd,2)))-pow(sigma*(1-fp),2));
            
            
        }
    }
    void getw3d()
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
                    
                    omega3d[i*m1+j*m2+m]= sqrt(pow(fd*fomega*interm,2)+interm*(pow(sigma*(1-fp*beta),2)/beta+pow(fomega,2)*(1-pow(fd*beta,2))/beta)-pow(sigma*(1-fp*beta),2));
                }
                
            }
        }
    }
    
    //==================================
    //==================================
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int
                    currentPitchWheelPosition)
    {
        //the keyboard-voice activation interface, should write different excitation signals here.
        trig = 1;
        t=0;
        nsamp=0;
        level=velocity;
        //map keyboard to frequency
        frequency= MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    };
    
    //==================================
    void stopNote (float velocity, bool allowTailOff)
    {
        allowTailOff=true;
        
        if(velocity ==0)//allow next note to come in
            t=0;
        clearCurrentNote();
        
    };
    //==================================
    bool isVoiceActive()
    {
        
    };
    //==================================
    void pitchWheelMoved (int newPitchWheelValue)
    {
        
    };
    //==================================
    void controllerMoved (int controllerNumber, int newControllerValue)
    {
        
    };
    //==================================
    void aftertouchChanged (int newAftertouchValue)
    {
        
    };
    //==================================
    void channelPressureChanged (int newChannelPressureValue)
    {
        
    };
    //==================================
    void renderNextBlock (AudioBuffer< float > &outputBuffer, int startSample, int numSamples)
    {
        //callback function
        
        
        for(int sample=0;sample<numSamples;++sample)
        {
            
            //put the synthesized drum sound here
            double drumSound=finaloutput(sample);
            //std::cout<<"drum Sound "<<drumSound<<"\n";
            
            for(int channel=0;channel<outputBuffer.getNumChannels();++channel)
            {
                
                outputBuffer.addSample(channel,  startSample, drumSound );//put whatever sound synethesized here.
                
            }
            ++startSample;
        }
        
        
        
    };
    //==================================
    void renderNextBlock (AudioBuffer< double > &outputBuffer, int startSample, int numSamples)
    {
        
        
    };
    //==================================
    void setCurrentPlaybackSampleRate (double newRate)
    {
        
    };
    //==================================
    bool isPlayingChannel (int midiChannel)
    {
        
    };
    //==================================
    double getSampleRate()
    {
        
    };
    //==================================
    bool isKeyDown()
    {
        
    };
    //==================================
    void setKeyDown(bool isNowDown)
    {
        
    };
    //==================================
    bool isPlayingButReleased()
    {
        
    };
    //==================================
    bool wasStartedBefore(const SynthesiserVoice& other)
    {
        
    };
    //==================================
    //==================================
    //==================================
    //==================================
    
    
private:
   // StringModelAudioProcessor& processor;
    double level;
    double frequency;
    int trig;
    double output;
    double t;
    int nsamp;
    double sr=44100;
    double sinlookup[62832];
    float ftau;
    float fa;
    float fa2;
    float fp;
    float fomega;
    float fd;
    float dim;
    int tau=300;
    int m1=5;   //can't be bigger than MAX_M1
    int m2=5;
    int m3=5;
    float k[MAX_M1*MAX_M2];//m1*m2
    float omega[25];
    float sigma[25];
    float decayamp1[MAX_M1];
    float decayampn1[MAX_M1];
    float decayamp[25];
    float decayampn[25];
    float decayamp3[125];
    float decayampn3[125];
    float k3d[125];//m1*m2
    float k1d[MAX_M1];
    float omega3d[125];
    float omega1d[MAX_M1];
    float sigma3d[125];
    float sigma1d[MAX_M1];
    float f1[5];//m1
    float f2[5];//m2
    float f3[5];//m3
    float maxh;//the max of h for each set of parameters
    // float test[25];
    float fx1[301];//tau
    float fx2[301];
    float fx3[301];
    //double drumSound[512];
    double gain;
    float l1;
    float fthick;
    float lengthscale = M_PI;
    float r1=0.5,r2=0.5;
};
