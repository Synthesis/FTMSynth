/*
  ==============================================================================

    SynthVoice.cpp
    Created: 10 Oct 2018 3:46:36pm
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


// some function that grabs value from the slider, and then either returns or set the signal of my synthesized drum sound
// IMPORTANT NOTE: the parameters need to be passed as std::atomic<float>*, from tree.getRawParameterValue("name"),
//                 otherwise they'll be applied AFTER the next note press, instead of before, which means the
//                 parameters will be updated one hit too late, which is *not* what we want.
void SynthVoice::getcusParam(std::atomic<float>* algo,
                             std::atomic<float>* volume,
                             std::atomic<float>* attack,
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
    nextAtk = attack->load();

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
    double s = 0.4;  // standard deviation
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
                integ += (fx1[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx1[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;  // (f(b)+f(a))*(b-a)/2
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
                integ += (fx2[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx2[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;  // (f(b)+f(a))*(b-a)/2
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
                integ += (fx3[i+1]*sin((i+1)*h*M_PI*(j+1)/l) + fx3[i]*sin(i*h*M_PI*(j+1)/l))*h/2.0;  // (f(b)+f(a))*(b-a)/2
            }
            f3[j] = 2*integ/l;
        }
    }
}

// intermediate variables
// sigma
void SynthVoice::ivan_getSigma(double _tau, double p)
{
    double fsigma = -1/_tau;

    // 1D
    if (dim == 0)
    {
        for (int i=0; i<m1; i++)
        {
            sigma[i] = fsigma*(1+p*(pow(i+1,2)-1));
            decayamp[i] = exp(sigma[i]/sr);
        }
    }
    // 2D
    else if (dim == 1)
    {
        double fbeta = fa + 1/fa;
        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                sigma[i+m1*j] = fsigma*(1+p*(pow(i+1,2)*fa+pow(j+1,2)/fa-fbeta));
                decayamp[i+m1*j] = exp(sigma[i+m1*j]/sr);
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        double fbeta = fa*fa2 + fa/fa2 + fa2/fa;
        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    sigma[i+m1*(j+m2*k)] = fsigma*(1+p*(pow(i+1,2)*fa*fa2 + pow(j+1,2)*fa2/fa + pow(k+1,2)*fa/fa2 - fbeta));
                    decayamp[i+m1*(j+m2*k)] = exp(sigma[i+m1*(j+m2*k)]/sr);
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
        double fsigma = -1/ftau;
        for (int i=0; i<m1; i++)
        {
            double interm = pow(i+1,2);  // M^2
            omega[i] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(fsigma*(1-p), 2) + pow(fomega,2)*(1-pow(fd, 2))) - pow(fsigma*(1-p), 2));

            // remove aliasing by checking whether the mode frequency is above Nyquist
            mode_rejected[i] = ((omega[i]/(2*M_PI)) >= (sr/2));
        }
    }
    // 2D
    else if (dim == 1)
    {
        double fbeta = fa + 1/fa;
        double fsigma = -1/ftau;

        int index = 0;

        for (int j=0; j<m2; j++)
        {
            for (int i=0; i<m1; i++)
            {
                index = i+m1*j;

                double interm = pow(i+1,2)*fa + pow(j+1,2)/fa;
                omega[index] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(fsigma*(1-p*fbeta), 2)/fbeta + pow(fomega, 2)*(1-pow(fd*fbeta, 2))/fbeta) - pow(fsigma*(1-p*fbeta), 2));

                // aliasing removal
                mode_rejected[index] = ((omega[index]/(2*M_PI)) >= (sr/2));
            }
        }
    }
    // 3D
    else if (dim == 2)
    {
        double fbeta = fa*fa2 + fa/fa2 + fa2/fa;
        double fsigma = -1/ftau;

        int index = 0;

        for (int k=0; k<m3; k++)
        {
            for (int j=0; j<m2; j++)
            {
                for (int i=0; i<m1; i++)
                {
                    index = i+m1*(j+m2*k);

                    double interm = pow(i+1,2)*fa*fa2 + pow(j+1,2)*fa2/fa + pow(k+1,2)*fa/fa2;
                    omega[index] = sqrt(pow(fd*fomega*interm, 2) + interm * (pow(fsigma*(1-p*fbeta), 2)/fbeta + pow(fomega, 2)*(1-pow(fd*fbeta, 2))/fbeta) - pow(fsigma*(1-p*fbeta), 2));

                    // aliasing removal
                    mode_rejected[index] = ((omega[index]/(2*M_PI)) >= (sr/2));
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
            knd[i] = f1[i] * sin((i+1)*x1*M_PI/l1) / omega[i];
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
                knd[i+m1*j] = f1[i]*f2[j] * sin((i+1)*x1*M_PI/l1) * sin((j+1)*x2*M_PI/l2) / omega[i+m1*j];
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
                    knd[i+m1*(j+m2*k)] = f1[i]*f2[j]*f3[k] * sin((i+1)*x1*M_PI/l1) * sin((j+1)*x2*M_PI/l2) * sin((k+1)*x3*M_PI/l3) / omega[i+m1*(j+m2*k)];
                }
            }
        }
    }
}


void SynthVoice::rabenstein_getCoefficients(double _tau, double p)
{
    double l0 = M_PI;  // constant

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

            beta[i] = EI * n2 + T * n;
            alpha[i] = (d1 - d3 * n) / 2;

            decayamp[i] = exp(-alpha[i]/sr);
        }

        fN = M_PI / 4;
    }
    // 2D
    else if (dim == 1)
    {
        double l2 = M_PI*fa;
        double fbeta = fa + 1/fa;

        double EI = pow(fd*fomega*fa, 2) + pow(p*fa/_tau, 2);
        double T = (fa * (1/fbeta - p*p*fbeta) / _tau*_tau
                + fa * fomega*fomega * (1/fbeta - fd*fd * fbeta));

        double d1 = 2 * (1 - p*fbeta) / _tau;
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

                beta[index] = EI * n2 + T * n;
                alpha[index] = (d1 - d3 * n) / 2;

                decayamp[index] = exp(-alpha[index]/sr);
            }
        }

        fN = M_PI * l2 / 4;
    }
    // 3D
    else if (dim == 2)
    {
        double l2 = M_PI*fa;
        double l3 = M_PI*fa2;
        double fbeta = fa*fa2 + fa/fa2 + fa2/fa;

        double EI = pow(fd*fomega*fa*fa2, 2) + pow(p*fa*fa2/_tau, 2);
        double T = (fa*fa2 * (1/fbeta - p*p*fbeta) / _tau*_tau
                + fa*fa2 * fomega*fomega * (1/fbeta - fd*fd * fbeta));

        double d1 = 2 * (1 - p*fbeta) / _tau;
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

                    beta[index] = EI * n2 + T * n;
                    alpha[index] = (d1 - d3 * n) / 2;

                    decayamp[index] = exp(-alpha[index]/sr);
                }
            }
        }

        fN = M_PI * l2 * l3 / 8;
    }
}

// get coefficient omega for the impulse response
void SynthVoice::rabenstein_getw()
{
    int maxIndex = 0;
    if (dim >= 0) maxIndex = m1;
    if (dim >= 1) maxIndex *= m2;
    if (dim >= 2) maxIndex *= m3;

    for (int i=0; i<maxIndex; i++)
    {
        omega[i] = sqrt(abs(beta[i] - alpha[i]*alpha[i]));
        mode_rejected[i] = ((omega[i]/(2*M_PI)) > (sr/2));
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
            knd[i] = sin((i+1)*M_PI*r1);
            yi[i] = level * knd[i] / omega[i];
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

                knd[index] = sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2);
                yi[index] = level * knd[index] / omega[index];
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

                    knd[index] = sin((i+1)*M_PI*r1) * sin((j+1)*M_PI*r2) * sin((k+1)*M_PI*r3);
                    yi[index] = level * knd[index] / omega[index];
                }
            }
        }
    }
}


// findmax functions find value of first sample and scale everything else based on this value
void SynthVoice::findmax()
{
    double h = 0;

    int maxIndex = 0;
    if (dim >= 0) maxIndex = m1;
    if (dim >= 1) maxIndex *= m2;
    if (dim >= 2) maxIndex *= m3;

    double coef, decay;
    for (int i=0; i<maxIndex; i++)
    {
        coef = 1;
        decay = 0;
        if (currentAlgorithm == Algorithm::ivan)
        {
            decay = sigma[i];
        }
        else if (currentAlgorithm == Algorithm::rabenstein)
        {
            coef = yi[i];
            decay = -alpha[i];
        }
        h += coef * knd[i] * exp(decay*M_PI_2 / omega[i]);
    }
    if (currentAlgorithm == Algorithm::rabenstein) h /= fN*level;

    if (h == 0) h = 1;

    maxh = h;
}

void SynthVoice::initDecayampn()
{
    int maxIndex = 0;
    if (dim >= 0) maxIndex = m1;
    if (dim >= 1) maxIndex *= m2;
    if (dim >= 2) maxIndex *= m3;

    for (int i=0; i<maxIndex; i++)
        decayampn[i] = 1.0;
}


//==================================
void SynthVoice::prepareActiveModes()
{
    activePhases.clear();
    activeIncrements.clear();
    activeGains.clear();
    activeDecays.clear();
    activeEnvStates.clear();
    activePeriodCount.clear();

    if (!trig) return;

    double lvl = level;
    if (currentAlgorithm == Algorithm::rabenstein) lvl = 1.0 / fN;
    double gainScale = lvl / maxh;

    int maxIndex = 0;
    if (dim >= 0) maxIndex = m1;
    if (dim >= 1) maxIndex *= m2;
    if (dim >= 2) maxIndex *= m3;

    for (int i = 0; i < maxIndex; i++)
    {
        if (!mode_rejected[i])
        {
            // Initial phase is always 0 for now as we start at t=0
            activePhases.push_back(0);

            // Calculate phase increment per sample
            // omega is angular frequency (rad/s)
            // Map 2pi -> 2^32 (4294967296.0)
            // inc = (omega / (sr * 2.0 * M_PI)) * 4294967296.0
            double increment = (omega[i] / (sr * 2.0 * M_PI)) * 4294967296.0;
            activeIncrements.push_back(static_cast<uint32_t>(increment));

            // Combined Gain
            double y = 1.0;
            if (currentAlgorithm == Algorithm::rabenstein) y = yi[i];
            activeGains.push_back(knd[i] * y * gainScale);

            // Decay
            activeDecays.push_back(decayamp[i]);
            activeEnvStates.push_back(1.0);  // Starts at 1.0
            activePeriodCount.push_back(0);  // New note, period 0
        }
    }
}

void SynthVoice::updateActiveDecays()
{
    // This is called when decay rates change (e.g. release, sample rate change)
    // We need to re-match the active modes with their new decay rates.
    // Since activeModes list order corresponds to the linear iteration of valid modes,
    // we can re-iterate to find them.

    int activeIdx = 0;
    int maxIndex = 0;
    if (dim >= 0) maxIndex = m1;
    if (dim >= 1) maxIndex *= m2;
    if (dim >= 2) maxIndex *= m3;

    for (int i = 0; i < maxIndex; i++)
    {
        if (!mode_rejected[i])
        {
            if (activeIdx < activeDecays.size())
            {
                activeDecays[activeIdx] = decayamp[i];
                activeIdx++;
            }
        }
    }
}

//==================================
// this function synthesizes the signal value at each sample
void SynthVoice::synthesizeBlock(int numSamples)
{
    // ensure scratch buffer is large enough
    if (buffer.size() < (size_t)numSamples)
        buffer.resize(numSamples);

    // clear scratch buffer
    std::fill(buffer.begin(), buffer.begin() + numSamples, 0.0);

    // block processing: iterate active modes
    size_t numActive = activePhases.size();

    // update pitch bend if needed (assuming constant per block for now)
    double currentPitchMultiplier = pow(2.0, pitchBend);
    bool applyGainScaling = (currentAlgorithm == Algorithm::rabenstein);
    uint32_t nyquistInc = 0x80000000;  // corresponding to SR/2

    for (size_t i = 0; i < numActive; i++)
    {
        // apply pitch bend to increment
        uint64_t largeInc = static_cast<uint64_t>(static_cast<double>(activeIncrements[i]) * currentPitchMultiplier);

        // anti-aliasing check: if frequency exceeds Nyquist, skip this mode
        if (largeInc >= nyquistInc) continue;

        uint32_t inc = static_cast<uint32_t>(largeInc);
        uint32_t phase = activePhases[i];

        double gain = activeGains[i];
        if (applyGainScaling) gain /= currentPitchMultiplier;

        double decay = activeDecays[i];
        double amp = activeEnvStates[i];

        for (int s = 0; s < numSamples; s++)
        {
            // direct lookup (truncation) for 256k LUT
            // top 18 bits = integer index
            // bottom 14 bits = fractional part (discarded)
            uint32_t index = phase >> 14;

            double value = sinLUT[index];

            // Apply Duration-Based Attack Windowing
            if (atk > 0.0)
            {
               // Check if we are still within the attack duration
               // Use precise floating point position: periods + fractional_phase
               double currentPos = activePeriodCount[i] + (phase / 4294967296.0);

               if (currentPos < atk)
               {
                   // Window function: sin^2( pi * t / (2 * dur) )
                   // Maps t=0 -> 0, t=dur -> 1
                   // Argument for sin is (pi/2) * (t/dur)
                   // Map to LUT index [0 .. LUT_SIZE/4]

                   double ratio = currentPos / atk;

                   // LUT Size is 0x40000. Quarter is 0x10000.
                   uint32_t attackIndex = static_cast<uint32_t>(ratio * 0x10000);

                   // Clamp index just in case floating point errors push it slightly over
                   if (attackIndex > 0x10000) attackIndex = 0x10000;

                   double w = sinLUT[attackIndex];
                   value *= (w * w);
               }
            }

            buffer[s] += gain * amp * value;

            uint32_t nextPhase = phase + inc;
            if (nextPhase < phase)
            {
               if (activePeriodCount[i] < 255) // prevent overflow wrap-around
                   activePeriodCount[i]++;
            }
            phase = nextPhase;
            amp *= decay;
        }

        // write back state
        activePhases[i] = phase;
        activeEnvStates[i] = amp;
    }
}

void SynthVoice::advanceTime(int numSamples)
{
    nsamp += numSamples;
    t = nsamp / sr;

    if (t >= dur)
    {
        trig = false;
        clearCurrentNote();
    }
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

    atk = nextAtk;

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
        rabenstein_getw();
        rabenstein_getK();
    }
    findmax();
    initDecayampn();

    t = 0;
    nsamp = 0;
    trig = true;
    setKeyDown(true);

    prepareActiveModes();
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
            double elapsed = t/dur;  // percentage of the elapsed duration
            double remaining = 1.0 - elapsed;  // percentage of the remaining duration
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
            updateActiveDecays();
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
    if (!trig) return;

    synthesizeBlock(numSamples);

    // copy to output
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
    {
        float* outData = outputBuffer.getWritePointer(channel, startSample);
        for (int s = 0; s < numSamples; s++)
        {
            outData[s] += (float)(buffer[s] * mainVolume);
        }
    }

    advanceTime(numSamples);
}

//==================================
void SynthVoice::renderNextBlock(AudioBuffer<double> &outputBuffer, int startSample, int numSamples)
{
    if (!trig) return;

    synthesizeBlock(numSamples);

    // copy to output
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
    {
        double* outData = outputBuffer.getWritePointer(channel, startSample);
        for (int s = 0; s < numSamples; s++)
        {
            outData[s] += buffer[s] * mainVolume;
        }
    }

    advanceTime(numSamples);
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
        rabenstein_getw();
    }
    updateActiveDecays();
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
