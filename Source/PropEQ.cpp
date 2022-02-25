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
    for (int i = 0; i < N_EQ; i++) {
        states[i].x1 = 0.f;
        states[i].x2 = 0.f;
        states[i].y0 = 0.f;
        states[i].y1 = 0.f;
        states[i].y2 = 0.f;
    }
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
    if (input != 0.f)
        int test = 0;
    x0 = input;
    g2 = powf(g[0], 0.5f);
    states[0].y0 = g2 *
        1/q0[0] * (p0[0] * x0 + p1[0] * states[0].x1 + p2[0] * states[0].x2
        - q1[0] * states[0].y1 - q2[0] * states[0].y2);
    updateState(0);


    for (int i = 1; i < N_EQ - 1; i++) {
        float t1 = 1 / q0[i];
        float t2 = p0[i] * x0 + p1[i] * states[i].x1 + p2[i] * states[i].x2;
        float t3 = q1[i] * states[i].y1 - q2[i] * states[i].y2;
        states[i].y0 = 1/q0[i] * (p0[i] * x0 + p1[i] * states[i].x1 + p2[i] * states[i].x2
            - q1[i] * states[i].y1 - q2[i] * states[i].y2);
        updateState(i);
    }

    states[N_EQ - 1].y0 = g[N_EQ - 1] * q0[N_EQ - 1] * (p0[N_EQ - 1] * x0 - p1[N_EQ - 1] * states[N_EQ - 1].x1 - p2[N_EQ - 1] * states[N_EQ - 1].x2
        + q1[N_EQ - 1] * states[N_EQ - 1].y1 + q2[N_EQ - 1] * states[N_EQ - 1].y2);
    updateState(N_EQ - 1);

    float output = 0.0f;

    for (int i = 0; i < N_EQ; i++) {
        output += states[i].y0;
    }

    output *= g0;

    return output;
}

void PropEQ::setPolesAndZeros()
{
    // Low shelf EQ
    omega = tanf(freqToW(centerFreqs[0]) / 2);
    omega2 = powf(omega, 2.f);
    g2 = powf(g[0], 0.5f);
    g4 = powf(g[0], 0.25f);
    p0[0] = g2 * omega2 + sqrt(2) * omega * g4 + 1;
    p1[0] = 2 * g2 * omega2 - 2;
    p2[0] = g2 * omega2 - sqrt(2) * omega * g4 + 1;
    q0[0] = g2 + sqrt(2) * omega * g4 + omega2;
    q1[0] = 2 * omega2 - 2 * g2;
    q2[0] = g2 - sqrt(2) * omega * g4 + omega2;

    // Notch EQs
    for (int i = 1; i < N_EQ - 1; i++) {
        float wc = freqToW(centerFreqs[i]);
        float B = wc / Q;
        float t = tanf(B / 2.f);
        float c = cosf(wc);
        g2 = powf(g[i], 0.5f);
        p0[i] = g2 + g[i] * t;
        p1[i] = -2 * g2 * c;
        p2[i] = g2 - g[i] * t;
        q0[i] = g2 + t;
        q1[i] = -2 * g2 * c;
        q2[i] = g2 + t;
    }

    // High shelf EQ
    int idx = N_EQ - 1;
    omega = tan(freqToW(centerFreqs[idx]) / 2);
    g2 = powf(g[idx], 0.5f);
    g4 = powf(g[idx], 0.25f);
    p0[idx] = g2 * omega2 + sqrt(2) * omega * g4 + 1;
    p1[idx] = 2 * g2 * omega2 - 2;
    p2[idx] = g2 * omega2 - sqrt(2) * omega * g4 + 1;
    q0[idx] = g2 + sqrt(2) * omega * g4 + omega2;
    q1[idx] = 2 * omega2 - 2 * g2;
    q2[idx] = g2 - sqrt(2) * omega * g4 + omega2;
}

float PropEQ::freqToW(float frequency)
{
    return float(frequency / samplerate);
}

void PropEQ::updateState(int idx)
{
    states[idx].x2 = states[idx].x1;
    states[idx].x1 = x0;
    states[idx].y2 = states[idx].y1;
    states[idx].y1 = states[idx].y0;
}
