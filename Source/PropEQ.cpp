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
    x1 = x2 = y0 = y1 = y2 = 0.0f;
    setPolesAndZeros();
}

void PropEQ::setGainVector(float gainVector[N_EQ])
{
    for (int i = 0; i < N_EQ; i++) {
        g[i] = gainVector[i];
    }
}

float PropEQ::process(float input)
{
    if (input > 0.f)
        int test = 0;
    x0 = input;
    HLS = pow(g[0],  1.f / 2.f) *
        1/q0[0] * (p0[0] * x0 + p1[0] * x1 + p2[0] * x2
        - q1[0] * y1 - q2[0] * y2);

    for (int i = 0; i < N_EQ - 2; i++) {
        HPN[i] = 1/q0[i] * (p0[i] * x0 + p1[i] * x1 + p2[i] * x2
            - q1[i] * y1 - q2[i] * y2);
    }

    HHS = g[N_EQ - 1] * q0[N_EQ - 1] * (p0[N_EQ - 1] * x0 - p1[N_EQ - 1] * x1 - p2[N_EQ - 1] * x2
        + q1[N_EQ - 1] * y1 + q2[N_EQ - 1] * y2);;

    y0 = HLS;
    for (int i = 0; i < N_EQ - 2; i++) {
        y0 += HPN[i];
    }
    y0 += HHS;

    y0 *= g0;

    x2 = x1;
    x1 = x0;
    y2 = y1;
    y1 = y0;

    return y0;
}

void PropEQ::setPolesAndZeros()
{
    // Low shelf EQ
    double omega = tan(freqToW(centerFreqs[0]) / 2);
    p0[0] = pow(g[0], 1.f / 2.f) * pow(omega, 2) + sqrt(2) * omega * pow(g[0],  1.f / 4.f) + 1;
    p1[0] = 2 * pow(g[0], 1.f / 2.f) * pow(omega, 2) - 2;
    p2[0] = pow(g[0], 1.f / 2.f) * pow(omega, 2) - sqrt(2) * omega * pow(g[0],  1.f / 4.f) + 1;
    q0[0] = pow(g[0],  1.f / 2.f) + sqrt(2) * omega * pow(g[0],  1.f / 4.f) + pow(omega, 2);
    q1[0] = 2 * pow(g[0],  1.f / 2.f) * pow(omega, 2) - 2;
    q2[0] = pow(g[0],  1.f / 2.f) - sqrt(2) * omega * pow(g[0],  1.f / 4.f) + pow(omega, 2);

    // Notch EQs
    for (int i = 1; i < N_EQ - 1; i++) {
        float Q = 1.0;
        float wc = freqToW(centerFreqs[i]);
        float B = wc / Q;
        p0[i] = pow(g[i], 1.f / 2.f) + g[i] * tan(B / 2);
        p1[i] = -2 * pow(g[i],  1.f / 2.f) * cos(wc);
        p2[i] = pow(g[i],  1.f / 2.f) - g[i] * tan(B / 2);
        q0[i] = pow(g[i],  1.f / 2.f) + tan(B / 2);
        q1[i] = -2 * pow(g[i],  1.f / 2.f) * cos(wc);
        q2[i] = pow(g[i],  1.f / 2.f) + tan(B / 2);
    }

    // High shelf EQ
    int idx = N_EQ - 1;
    omega = tan(freqToW(centerFreqs[idx]) / 2);
    p0[idx] = pow(g[idx],  1.f / 2.f) * pow(omega, 2) + sqrt(2) * omega * pow(g[idx],  1.f / 4.f) + 1;
    p1[idx] = 2 * pow(g[idx],  1.f / 2.f) * pow(omega, 2) - 2;
    p2[idx] = pow(g[idx],  1.f / 2.f) * pow(omega, 2) - sqrt(2) * omega * pow(g[idx],  1.f / 4.f) + 1;
    q0[idx] = pow(g[idx],  1.f / 2.f) + sqrt(2) * omega * pow(g[idx],  1.f / 4.f) + pow(omega, 2);
    q1[idx] = 2 * pow(g[idx],  1.f / 2.f) * pow(omega, 2) - 2;
    q2[idx] = pow(g[idx],  1.f / 2.f) - sqrt(2) * omega * pow(g[idx],  1.f / 4.f) + pow(omega, 2);
}

float PropEQ::freqToW(float frequency)
{
    return float(frequency / samplerate);
}
