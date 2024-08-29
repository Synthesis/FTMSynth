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
    nexta = alpha1->load();
    nexta2 = alpha2->load();

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
    double s = 0.4; // standard deviation
    // 1D
    if (dim >= 0)
    {
        double h = M_PI/tau;
        for (int i = 0; i < tau+1; i++)
        {
            fx1[i] = (1 / (s * sqrt(2*M_PI))) * exp(-0.5 * pow((i*h - M_PI*r1) / s, 2.0));
        }
    }
    // 2D
    if (dim >= 1)
    {
        double h = fa*M_PI/tau;
        for (int i = 0; i < tau+1; i++)
        {
            fx2[i] = (1 / (s * sqrt(2*M_PI))) * exp(-0.5 * pow((i*h - fa*M_PI*r2) / s, 2.0));
        }
    }
    // 3D
    if (dim >= 2)
    {
        double h = fa2*M_PI/tau;
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
    double integ;

    // 1D
    if (dim >= 0)
    {
        double l = M_PI;
        int m = m1;
        double h = l / tau;
        for (int j=0; j<m; j++)
        {
            integ = 0;
            for (int i=0; i<tau; i++)
            {
                integ += (fx1[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx1[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0; //(f(b)+f(a))*(b-a)/2
            }
            f1[j] = 2*integ/l;
        }
    }
    // 2D
    if (dim >= 1)
    {
        double l = fa*M_PI;
        int m = m2;
        double h = l / tau;
        for (int j=0; j<m; j++)
        {
            integ = 0;
            for (int i=0; i<tau; i++)
            {
                integ += (fx2[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx2[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0; //(f(b)+f(a))*(b-a)/2
            }
            f2[j] = 2*integ/l;
        }
    }
    // 3D
    if (dim >= 2)
    {
        double l = fa2*M_PI;
        int m = m3;
        double h = l / tau;
        for (int j=0; j<m; j++)
        {
            integ = 0;
            for (int i=0; i<tau; i++)
            {
                integ += (fx3[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx3[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0; //(f(b)+f(a))*(b-a)/2
            }
            f3[j] = 2*integ/l;
        }
    }
}

// intermediate variables
// sigma
void SynthVoice::ivan_getSigma(double _tau, double p)
{
    double sigma1 = -1/_tau;

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
        double beta = fa + 1/fa;
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
        double beta = fa*fa2 + fa/fa2 + fa2/fa;
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
void SynthVoice::ivan_getw(double p)
{
    // 1D
    if (dim == 0)
    {
        double sigma = -1/ftau;
        for (int i=0; i<m1; i++)
        {
            double interm = pow(i+1,2); //M^2
            omega1d[i] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(sigma*(1-p), 2) + pow(fomega,2)*(1-pow(fd, 2))) - pow(sigma*(1-p), 2));
        }
    }
    // 2D
    else if (dim == 1)
    {
        double beta = fa + 1/fa;
        double sigma = -1/ftau;
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                double interm = pow(i+1,2)*fa + pow(j+1,2)/fa;
                omega2d[i+m1*j] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(sigma*(1-p*beta), 2)/beta + pow(fomega, 2)*(1-pow(fd*beta, 2))/beta) - pow(sigma*(1-p*beta), 2));
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        double beta = fa*fa2 + fa/fa2 + fa2/fa;
        double sigma = -1/ftau;
        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    double interm = pow(i+1,2)*fa*fa2 + pow(j+1,2)*fa2/fa + pow(k+1,2)*fa/fa2;
                    omega3d[i+m1*(j+m2*k)] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(sigma*(1-p*beta), 2)/beta + pow(fomega, 2)*(1-pow(fd*beta, 2))/beta) - pow(sigma*(1-p*beta), 2));
                }
            }
        }
    }
}

// get coefficient k for the impulse response
void SynthVoice::ivan_getK()
{
    double l1 = M_PI;
    double x1 = l1*r1;

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
        double l2 = fa*M_PI;
        double x2 = l2*r2;

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
        double l2 = fa*M_PI;
        double l3 = fa2*M_PI;
        double x2 = l2*r2;
        double x3 = l3*r3;

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


void SynthVoice::rabenstein_getCoefficients(double _tau, double p)
{
    double l0 = M_PI; // constant

    // 1D
    if (dim == 0)
    {
        double EI = pow(fd*fomega, 2) + pow(p/_tau, 2);
        double T = ((1 - p*p) / _tau*_tau + fomega*fomega * (1 - fd*fd));

        double d1 = 2 * (1 - p) / _tau;
        double d3 = -2 * p / _tau;

        double n, n2;

        for (int i=0; i<m1; i++)
        {
            n = pow(i+1, 2);
            n2 = n*n;

            beta1d[i] = EI * n2 + T * n;
            alpha1d[i] = (d1 - d3 * n) / 2;

            decayamp1[i] = exp(-alpha1d[i]/sr);
        }

        fN1d = M_PI / 4;
    }
    // 2D
    else if (dim == 1)
    {
        double l2 = M_PI*fa;
        double beta = fa + 1/fa;

        double EI = pow(fd*fomega*fa, 2) + pow(p*fa/_tau, 2);
        double T = (fa * (1/beta - p*p*beta) / _tau*_tau
                + fa * fomega*fomega * (1/beta - fd*fd * beta));

        double d1 = 2 * (1 - p*beta) / _tau;
        double d3 = -2 * p * fa / _tau;

        int index = 0;
        double n, n2;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;

                n = pow((i+1)*M_PI/l0, 2) + pow((j+1)*M_PI/l2, 2);
                n2 = n*n;

                beta2d[index] = EI * n2 + T * n;
                alpha2d[index] = (d1 - d3 * n) / 2;

                decayamp2[index] = exp(-alpha2d[index]/sr);
            }
        }

        fN2d = M_PI * l2 / 4;
    }
    // 3D
    else if (dim == 2)
    {
        double l2 = M_PI*fa;
        double l3 = M_PI*fa2;
        double beta = fa*fa2 + fa/fa2 + fa2/fa;

        double EI = pow(fd*fomega*fa*fa2, 2) + pow(p*fa*fa2/_tau, 2);
        double T = (fa*fa2 * (1/beta - p*p*beta) / _tau*_tau
                + fa*fa2 * fomega*fomega * (1/beta - fd*fd * beta));

        double d1 = 2 * (1 - p*beta) / _tau;
        double d3 = -2 * p * fa*fa2 / _tau;

        int index = 0;
        double n, n2;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    index = i+m1*(j+m2*k);

                    n = pow((i+1)*M_PI/l0, 2) + pow((j+1)*M_PI/l2, 2) + pow((k+1)*M_PI/l3, 2);
                    n2 = n*n;

                    beta3d[index] = EI * n2 + T * n;
                    alpha3d[index] = (d1 - d3 * n) / 2;

                    decayamp3[index] = exp(-alpha3d[index]/sr);
                }
            }
        }

        fN3d = M_PI * l2 * l3 / 8;
    }
}

// get coefficient omega for the impulse response
void SynthVoice::rabenstein_getOmega()
{
    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            omega1d[i] = sqrt(abs(beta1d[i] - alpha1d[i]*alpha1d[i]));
            mode_rejected1d[i] = ((omega1d[i] / (2*M_PI)) > (sr / 2));
        }
    }
    // 2D
    else if (dim == 1)
    {
        int index = 0;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;
                omega2d[index] = sqrt(abs(beta2d[index] - alpha2d[index]*alpha2d[index]));
                mode_rejected2d[index] = ((omega2d[index] / (2*M_PI)) > (sr / 2));
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        int index = 0;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    index = i+m1*(j+m2*k);
                    omega3d[index] = sqrt(abs(beta3d[index] - alpha3d[index]*alpha3d[index]));
                    mode_rejected3d[index] = ((omega3d[index] / (2*M_PI)) > (sr / 2));
                }
            }
        }
    }
}

// get coefficients k and y for the impulse response
void SynthVoice::rabenstein_getK()
{
    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            k1d[i] = sin((i+1)*M_PI*r1);
            yi1d[i] = level * k1d[i] / omega1d[i];
        }
    }
    // 2D
    else if (dim == 1)
    {
        int index = 0;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;

                k2d[index] = sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2);
                yi2d[index] = level * k2d[index] / omega2d[index];
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        int index = 0;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    index = i+m1*(j+m2*k);

                    k3d[index] = sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2) * sin((k+1)*M_PI*r3);
                    yi3d[index] = level * k3d[index] / omega3d[index];
                }
            }
        }
    }
}


// findmax functions find value of first sample and scale everything else based on this value
void SynthVoice::findmax()
{
    double h = 0;

    // 1D
    if (dim == 0)
    {
        double coef, decay;
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
        double coef, decay;
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
        double coef, decay;
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

void SynthVoice::initDecayampn()
{
    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
            decayampn1[i] = 1.0;
    }
    // 2D
    else if (dim == 1)
    {
        for (int j=0; j<m2; j++)
            for (int i=0; i<m1; i++)
                decayampn2[i+m1*j] = 1.0;
    }
    // 3D
    else if (dim == 2)
    {
        for (int k=0; k<m3; k++)
            for (int j=0; j<m2; j++)
                for (int i=0; i<m1; i++)
                    decayampn3[i+m1*(j+m2*k)] = 1.0;
    }
}


// this function synthesizes the signal value at each sample
double SynthVoice::ivan_finaloutput()
{
    if (trig == false) return 0;

    double h = 0;
    double lutScale = SIN_LUT_RESOLUTION / (2.0*M_PI);

    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            decayampn1[i] *= decayamp1[i];
            h += k1d[i] * decayampn1[i] * fastSin(omega1d[i]*t);
        }
    }
    // 2D
    else if (dim == 1)
    {
        int index = 0;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;
                // designate the exponential envelope
                decayampn2[index] *= decayamp2[index];
                // synthesize the sound at time t
                h += k2d[index] * decayampn2[index] * fastSin(omega2d[index]*t);
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        int index = 0;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    index = i+m1*(j+m2*k);
                    decayampn3[index] *= decayamp3[index];
                    h += k3d[index] * decayampn3[index] * fastSin(omega3d[index]*t);
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

    double h = 0;
    double lutScale = SIN_LUT_RESOLUTION / (2.0*M_PI);

    // synthesize the sound at time t
    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            if (!mode_rejected1d[i])
            {
                decayampn1[i] *= decayamp1[i];
                h += (yi1d[i] * decayampn1[i] * fastSin(omega1d[i]*t)) * k1d[i];
            }
        }
        h /= fN1d;
    }
    // 2D
    else if (dim == 1)
    {
        int index = 0;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;
                if (!mode_rejected2d[index])
                {
                    decayampn2[index] *= decayamp2[index];
                    h += (yi2d[index] * decayampn2[index] * fastSin(omega2d[index]*t)) * k2d[index];
                }
            }
        }
        h /= fN2d;
    }
    // 3D
    else if (dim == 2)
    {
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
                        decayampn3[index] *= decayamp3[index];
                        h += (yi3d[index] * decayampn3[index] * fastSin(omega3d[index]*t)) * k3d[index];
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
        double frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440);
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
    fa = nexta;
    fa2 = nexta2;

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
        rabenstein_getCoefficients(ftau, fp);
        rabenstein_getOmega();
        rabenstein_getK();
    }
    findmax();
    initDecayampn();

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
        if (bgate)
        {
            double elapsed = t/dur; // percentage of the elapsed duration
            double remaining = 1.0 - elapsed; // percentage of the remaining duration
            double newDur = log(1-frel) / log(1-0.075);
            dur = dur*elapsed + newDur*remaining;
        }
        if (bgate || bpGate)
        {
            double _tau = (bgate ? frel : ftau);
            double p = (bpGate ? fring : fp);
            if (currentAlgorithm == Algorithm::ivan)
            {
                ivan_getSigma(_tau, p);
            }
            else if (currentAlgorithm == Algorithm::rabenstein)
            {
                rabenstein_getCoefficients(_tau, p);
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
            float drumSound = float(ivan_finaloutput());
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
            float drumSound = float(rabenstein_finaloutput());
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
    double _tau = (((!bgate) || isKeyDown()) ? ftau : frel);
    double p = (((!bpGate) || isKeyDown()) ? fp : fring);
    if (currentAlgorithm == Algorithm::ivan)
    {
        ivan_getSigma(_tau, p);
        ivan_getw(p);
    }
    else if (currentAlgorithm == Algorithm::rabenstein)
    {
        rabenstein_getCoefficients(_tau, p);
        rabenstein_getOmega();
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
