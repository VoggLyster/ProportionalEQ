/*
  ==============================================================================

    PropEq.h
    Created: 22 Feb 2022 3:19:05pm
    Author:  svkly

  ==============================================================================
*/
#include <JuceHeader.h>
#define N_EQ 9

class PropEQ
{
public:
    PropEQ(double samplerate);
    ~PropEQ() {}

    void setGainVector(float gainVector[N_EQ]);
    float process(float input);

private:
    void setPolesAndZeros();
    float freqToW(float frequency);

    double samplerate;

    float g[N_EQ] = { 1.0, 1.0, 1.0, 1.0, 0.5, 0.1, 0.1, 0.1, 0.1 };
    float g0 = 0.1f;

    float p0[N_EQ];
    float p1[N_EQ];
    float p2[N_EQ];
    float q0[N_EQ];
    float q1[N_EQ];
    float q2[N_EQ];

    float y0, y1, y2;
    float x0, x1, x2;

    float HLS, HHS = 0.0f;
    float HPN[N_EQ - 2];

    float centerFreqs[N_EQ] = { 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
};

#pragma once
