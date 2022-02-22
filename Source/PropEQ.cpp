/*
  ==============================================================================

    PropEQ.cpp
    Created: 22 Feb 2022 3:45:10pm
    Author:  svkly

  ==============================================================================
*/

#include "PropEQ.h"

PropEQ::PropEQ(double _samplerate)
{
    samplerate = _samplerate;
}

void PropEQ::setGainVector(float gainVector[N_EQ])
{
    for (int i = 0; i < N_EQ; i++) {
        g[i] = gainVector[i];
    }
}

float PropEQ::process(float input)
{
    return 0.0f;
}

void PropEQ::setPolesAndZeros()
{
    // Low shelf EQ
    double omega = tan(freqToW(centerFreqs[0]) / 2);
    p0[0] = pow(g[0], 1 / 2) * pow(omega, 2) + sqrt(2) * omega * pow(g[0], 1 / 4) + 1;
    p1[0] = 2 * pow(g[0], 1 / 2) * pow(omega, 2) - 2;
    p2[0] = pow(g[0], 1 / 2) * pow(omega, 2) - sqrt(2) * omega * pow(g[0], 1 / 4) + 1;
    q0[0] = pow(g[0], 1 / 2) + sqrt(2) * omega * pow(g[0], 1 / 4) + pow(omega, 2);
    q1[0] = 2 * pow(g[0], 1 / 2) * pow(omega, 2) - 2;
    q2[0] = pow(g[0], 1 / 2) - sqrt(2) * omega * pow(g[0], 1 / 4) + pow(omega, 2);

    // Notch EQs
    for (int i = 1; i < N_EQ - 1; i++) {
        double Q = 1.0;
        double wc = freqToW(centerFreqs[i]);
        double B = wc / Q;
        p0[i] = pow(g[i], 1 / 2) + g[i] * tan(B / 2);
        p1[i] = -2 * pow(g[i], 1 / 2) * cos(wc);
        p2[i] = pow(g[i], 1 / 2) - g[i] * tan(B / 2);
        q0[i] = pow(g[i], 1 / 2) + tan(B / 2);
        q1[i] = -2 * pow(g[i], 1 / 2) * cos(wc);
        q2[i] = pow(g[i], 1 / 2) + tan(B / 2);
    }
}

float PropEQ::freqToW(float frequency)
{
    return float(frequency / samplerate);
}
