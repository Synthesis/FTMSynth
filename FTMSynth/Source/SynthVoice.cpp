/*
  ==============================================================================

    SynthVoice.cpp
    Created: 10 Oct 2018 3:46:36pm
    Authors: Lily H, Loïc J

  ==============================================================================
*/

#include "SynthVoice.h"

#include <cmath>

using namespace std;


bool SynthVoice::canPlaySound(SynthesiserSound* sound)
{
    // if succesfully cast sound into my own class, return true
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}


//==================================
void SynthVoice::computeSinLUT()
{
    for (int i = 0; i < SIN_LUT_RESOLUTION; i++)
    {
        sinLUT[i] = sin(i * 2.0 * M_PI / SIN_LUT_RESOLUTION);
    }
}

#define fastSin(x) sinLUT[int(x*lutScale) & SIN_LUT_MODULO]


// some function that grabs value from the slider, and then either returns or set the signal of my synthesized drum sound
// IMPORTANT NOTE: the parameters need to be passed as std::atomic<float>*, from tree.getRawParameterValue("name"),
//                 otherwise they'll be applied AFTER the next note press, instead of before, which means the
//                 parameters will be updated one hit too late, which is what we *don't* want.
void SynthVoice::getcusParam(std::atomic<float>* algo,
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
                             std::atomic<float>* dimensions)
{
    // this function fetches parameters from the customized GUI and calculates the
    // corresponding parameters in order to synthesize the sound
    nextAlgorithm = ((int(algo->load()) >= 1) ? Algorithm::rabenstein : Algorithm::ivan);
    mainVolume = volume->load();

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


// define f(x) as a gaussian distribution with mean at the middle point l/2
void SynthVoice::ivan_deff()
{
    float s=0.4; // standard deviation
    if (dim >= 0)
    {
        float h = M_PI/tau;
        for (int i = 0; i < tau+1; i++)
        {
            fx1[i] = (1 / (s * sqrt(2*M_PI))) * exp(-0.5 * pow((i*h - M_PI*r1) / s, 2.0));
        }
    }
    if (dim >= 1)
    {
        float h = fa*M_PI/tau;
        for (int i = 0; i < tau+1; i++)
        {
            fx2[i] = (1 / (s * sqrt(2*M_PI))) * exp(-0.5 * pow((i*h - fa*M_PI*r2) / s, 2.0));
        }
    }
    if (dim >= 2)
    {
        float h = fa2*M_PI/tau;
        for (int i = 0; i < tau+1; i++)
        {
            fx3[i] = (1 / (s * sqrt(2*M_PI))) * exp(-0.5 * pow((i*h - fa2*M_PI*r3) / s, 2.0));
        }
    }
}

// get coefficients of the integral f1m1 using trapezoid rule
void SynthVoice::ivan_getf()
{
    // integrate f(x)sin(mpix/l)dx from 0 to l using trapezoid rule
    float integ;

    if (dim >= 0)
    {
        float l = M_PI;
        int m = m1;
        float h = l / tau;
        for (int j=0; j<m; j++)
        {
            integ=0;
            for (int i=0; i<tau; i++)
            {
                integ += (fx1[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx1[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0; //(f(b)+f(a))*(b-a)/2
            }
            f1[j] = 2*integ/l;
        }
    }
    if (dim >= 1)
    {
        float l = fa*M_PI;
        int m = m2;
        float h = l / tau;
        for (int j=0; j<m; j++)
        {
            integ=0;
            for (int i=0; i<tau; i++)
            {
                integ += (fx2[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx2[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0; //(f(b)+f(a))*(b-a)/2
            }
            f2[j] = 2*integ/l;
        }
    }
    if (dim >= 2)
    {
        float l = fa2*M_PI;
        int m = m3;
        float h = l / tau;
        for (int j=0; j<m; j++)
        {
            integ=0;
            for (int i=0; i<tau; i++)
            {
                integ += (fx3[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx2[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;//(f(b)+f(a))*(b-a)/2
            }
            f3[j] = 2*integ/l;
        }
    }
}


// intermediate variables
// sigma
void SynthVoice::ivan_getSigma(float _tau, float p)
{
    float sigma1 = -1/_tau;

    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            sigma1d[i] = sigma1*(1+p*(pow(i+1,2)-1));
            decayamp1[i] = exp(sigma1d[i]/sr);
        }
    }
    // 2D
    else if (dim == 1)
    {
        float beta = fa + 1/fa;
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                sigma2d[i+m1*j] = sigma1*(1+p*(pow(i+1,2)*fa+pow(j+1,2)/fa-beta));
                decayamp2[i+m1*j] = exp(sigma2d[i+m1*j]/sr);
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        float beta = fa*fa2 + fa/fa2 + fa2/fa;
        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    sigma3d[i+m1*(j+m2*k)] = sigma1*(1+p*(pow(i+1,2)*fa*fa2 + pow(j+1,2)*fa2/fa + pow(k+1,2)*fa/fa2 - beta));
                    decayamp3[i+m1*(j+m2*k)] = exp(sigma3d[i+m1*(j+m2*k)]/sr);
                }
            }
        }
    }
}


// get coefficient omega for the impulse response
void SynthVoice::ivan_getw(float p)
{
    // 1D
    if (dim == 0)
    {
        float sigma = -1/ftau;
        for (int i=0; i<m1; i++)
        {
            float interm = pow(i+1,2);//M^2
            omega1d[i] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(sigma*(1-p), 2) + pow(fomega,2)*(1-pow(fd, 2))) - pow(sigma*(1-p), 2));
        }
    }
    // 2D
    else if (dim == 1)
    {
        float beta = fa + 1/fa;
        float sigma = -1/ftau;
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                float interm = pow(i+1,2)*fa + pow(j+1,2)/fa;
                omega2d[i+m1*j] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(sigma*(1-p*beta), 2)/beta + pow(fomega, 2)*(1-pow(fd*beta, 2))/beta) - pow(sigma*(1-p*beta), 2));
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        float beta = fa*fa2 + fa/fa2 + fa2/fa;
        float sigma = -1/ftau;
        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    float interm = pow(i+1,2)*fa*fa2 + pow(j+1,2)*fa2/fa + pow(k+1,2)*fa/fa2;
                    omega3d[i+m1*(j+m2*k)] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(sigma*(1-p*beta), 2)/beta + pow(fomega, 2)*(1-pow(fd*beta, 2))/beta) - pow(sigma*(1-p*beta), 2));
                }
            }
        }
    }
}


// get coefficient k for the impulse response
void SynthVoice::ivan_getK()
{
    float l1 = M_PI;
    float x1 = l1*r1;

    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            if ((omega1d[i]/(2*M_PI)) >= (sr/2.0))
                // remove aliasing by checking whether the mode frequency is above Nyquist
                k1d[i] = 0;
            else
                k1d[i] = f1[i] * sin((i+1)*x1*M_PI/l1) / omega1d[i];
        }
    }
    // 2D
    else if (dim == 1)
    {
        float l2 = fa*M_PI;
        float x2 = l2*r2;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                if ((omega2d[i+m1*j]/(2*M_PI)) >= (sr/2.0))
                    // aliasing removal
                    k2d[i+m1*j] = 0;
                else
                    k2d[i+m1*j] = f1[i]*f2[j] * sin((i+1)*x1*M_PI/l1) * sin((j+1)*x2*M_PI/l2) / omega2d[i+m1*j];
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        float l2 = fa*M_PI;
        float l3 = fa2*M_PI;
        float x2 = l2*r2;
        float x3 = l3*r3;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    if ((omega3d[i+m1*(j+m2*k)]/(2*M_PI)) >= (sr/2.0))
                        // aliasing removal
                        k3d[i+m1*(j+m2*k)] = 0;
                    else
                        k3d[i+m1*(j+m2*k)] = f1[i]*f2[j]*f3[k] * sin((i+1)*x1*M_PI/l1) * sin((j+1)*x2*M_PI/l2) * sin((k+1)*x3*M_PI/l3) / omega3d[i+m1*(j+m2*k)];
                }
            }
        }
    }
}


void SynthVoice::rabenstein_initialize()
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

        float S = pow(pow(fd*fomega*fa*fa2, 2) + pow(fp*fa*fa2/ftau, 2), 0.25);
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
        float coef, decay;
        for (int i=0; i<m1; i++)
        {
            coef = 1;
            decay = 0;
            if (currentAlgorithm == Algorithm::ivan)
            {
                decay = sigma1d[i];
            }
            else if (currentAlgorithm == Algorithm::rabenstein)
            {
                coef = yi1d[i];
                decay = -alpha1d[i];
            }
            h += coef * k1d[i] * exp(decay*M_PI_2 / omega1d[i]);
        }
        if (currentAlgorithm == Algorithm::rabenstein) h /= fN1d*level;
    }
    // 2D
    else if (dim == 1)
    {
        float coef, decay;
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                coef = 1;
                decay = 0;
                if (currentAlgorithm == Algorithm::ivan)
                {
                    decay = sigma2d[i+m1*j];
                }
                else if (currentAlgorithm == Algorithm::rabenstein)
                {
                    coef = yi2d[i+m1*j];
                    decay = -alpha2d[i+m1*j];
                }
                h += coef * k2d[i+m1*j] * exp(decay*M_PI_2 / omega2d[i+m1*j]);
            }
        }
        if (currentAlgorithm == Algorithm::rabenstein) h /= fN2d*level;
    }
    // 3D
    else if (dim == 2)
    {
        float coef, decay;
        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    coef = 1;
                    decay = 0;
                    if (currentAlgorithm == Algorithm::ivan)
                    {
                        decay = sigma3d[i+m1*(j+m2*k)];
                    }
                    else if (currentAlgorithm == Algorithm::rabenstein)
                    {
                        coef = yi3d[i+m1*(j+m2*k)];
                        decay = -alpha3d[i+m1*(j+m2*k)];
                    }
                    h += coef * k3d[i+m1*(j+m2*k)] * exp(decay*M_PI_2 / omega3d[i+m1*(j+m2*k)]);
                }
            }
        }
        if (currentAlgorithm == Algorithm::rabenstein) h /= fN3d*level;
    }
    //
    else
    {
        h = 1;
    }

    maxh = h;
}


// this function synthesizes the signal value at each sample
double SynthVoice::ivan_finaloutput()
{
    if (trig == false) return 0;

    float h = 0;
    float lutScale = SIN_LUT_RESOLUTION / (2.0*M_PI);

    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            if(nsamp==0){
                decayampn1[i] = decayamp1[i];
            }
            else{
                decayampn1[i] *= decayamp1[i];
            }

            // h += k1d[i] * decayampn1[i] * sin(omega1d[i]*t);
            h += k1d[i] * decayampn1[i] * sinLUT[int(omega1d[i]*t*lutScale) & SIN_LUT_MODULO];
        }
    }
    // 2D
    else if (dim == 1)
    {
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                // designate the exponential envelope
                if (nsamp == 0)
                    decayampn2[i+m1*j] = decayamp2[i+m1*j];
                else
                    decayampn2[i+m1*j] *= decayamp2[i+m1*j];

                // synthesize the sound at time t
                // h += k2d[i+m1*j] * decayampn2[i+m1*j] * sin(omega2d[i+m1*j]*t);
                h += k2d[i+m1*j] * decayampn2[i+m1*j] * sinLUT[int(omega2d[i+m1*j]*t*lutScale) & SIN_LUT_MODULO];
            }
        }
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
                    if(nsamp==0){
                        decayampn3[i+m1*(j+m2*k)] = decayamp3[i+m1*(j+m2*k)];
                    }
                    else{
                        decayampn3[i+m1*(j+m2*k)] *= decayamp3[i+m1*(j+m2*k)];
                    }
                    // h += k3d[i+m1*(j+m2*k)] * decayampn3[(i*m2+j)*m2+k] * sin(omega3d[i+m1*(j+m2*k)]*t);
                    h += k3d[i+m1*(j+m2*k)] * decayampn3[i+m1*(j+m2*k)] * sinLUT[int(omega3d[i+m1*(j+m2*k)]*t*lutScale) & SIN_LUT_MODULO];
                }
            }
        }
    }

    double output = (h*level) / maxh;

    nsamp += pow(2.0, pitchBend);
    t = nsamp/sr; // t advancing one sample

    if (t >= dur)
    {
        trig = false;
        clearCurrentNote();
    }

    return output * mainVolume;
}

// this function synthesizes the signal value at each sample
double SynthVoice::rabenstein_finaloutput()
{
    if (trig == false) return 0;

    float h = 0;
    float lutScale = SIN_LUT_RESOLUTION / (2.0*M_PI);

    // synthesize the sound at time t
    // 1D
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
    // 2D
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
    // 3D
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

    double output = h / maxh;

    nsamp += pow(2.0, pitchBend);
    t = nsamp/sr; // t advancing one sample

    if (t >= dur)
    {
        trig = false;
        clearCurrentNote();
    }

    return output * mainVolume;
}



//==================================
//==================================
void SynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound */*sound*/,
                           int currentPitchWheelPosition)
{
    currentAlgorithm = nextAlgorithm;
    dim = nextDim;

    level = velocity;
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

    if (currentAlgorithm == Algorithm::ivan)
    {
        ivan_deff();
        ivan_getf();

        ivan_getSigma(ftau, fp);
        ivan_getw(fp);
        ivan_getK();
    }
    else if (currentAlgorithm == Algorithm::rabenstein)
    {
        rabenstein_initialize();
    }
    findmax();

    t = 0;
    nsamp = 0;
    trig = true;
    setKeyDown(true);
}

//==================================
void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (!allowTailOff)
    {
        trig = false;
        dur = 0;
        clearCurrentNote();
    }
    else
    {
        if (currentAlgorithm == Algorithm::ivan)
        {
            if (bgate)
            {
                float elapsed = t/dur; // percentage of the elapsed duration
                float remaining = 1.0 - elapsed; // percentage of the remaining duration
                float newDur = log(1-frel) / log(1-0.075);
                dur = dur*elapsed + newDur*remaining;
            }
            if (bgate || bpGate)
            {
                float _tau = (bgate ? frel : ftau);
                float p = (bpGate ? fring : fp);
                ivan_getSigma(_tau, p);
                ivan_getw(p);
            }
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
void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}
//==================================
void SynthVoice::aftertouchChanged(int /*newAftertouchValue*/)
{
}
//==================================
void SynthVoice::channelPressureChanged(int /*newChannelPressureValue*/)
{
}

//==================================
void SynthVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    if (currentAlgorithm == Algorithm::ivan)
    {
        for (int sample=0; sample<numSamples; sample++)
        {
            double drumSound = ivan_finaloutput();
            for (int channel=0; channel<outputBuffer.getNumChannels(); channel++)
            {
                outputBuffer.addSample(channel, startSample+sample, drumSound);
            }
        }
    }
    else if (currentAlgorithm == Algorithm::rabenstein)
    {
        for (int sample=0; sample<numSamples; sample++)
        {
            double drumSound = rabenstein_finaloutput();
            for (int channel=0; channel<outputBuffer.getNumChannels(); channel++)
            {
                outputBuffer.addSample(channel, startSample+sample, drumSound);
            }
        }
    }
}

//==================================
void SynthVoice::renderNextBlock(AudioBuffer<double> &outputBuffer, int startSample, int numSamples)
{
    if (currentAlgorithm == Algorithm::ivan)
    {
        for (int sample=0; sample<numSamples; sample++)
        {
            double drumSound = ivan_finaloutput();
            for (int channel=0; channel<outputBuffer.getNumChannels(); channel++)
            {
                outputBuffer.addSample(channel, startSample+sample, drumSound);
            }
        }
    }
    else if (currentAlgorithm == Algorithm::rabenstein)
    {
        for (int sample=0; sample<numSamples; sample++)
        {
            double drumSound = rabenstein_finaloutput();
            for (int channel=0; channel<outputBuffer.getNumChannels(); channel++)
            {
                outputBuffer.addSample(channel, startSample+sample, drumSound);
            }
        }
    }
}

//==================================
void SynthVoice::setCurrentPlaybackSampleRate(double newRate)
{
    sr = newRate;
    if (currentAlgorithm == Algorithm::ivan)
    {
        float p = (((!bpGate) || isKeyDown()) ? fp : fring);
        ivan_getSigma(((!bgate) || isKeyDown()) ? ftau : frel, p);
        ivan_getw(p);
    }
    else if (currentAlgorithm == Algorithm::rabenstein)
    {
        rabenstein_initialize();
    }
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
