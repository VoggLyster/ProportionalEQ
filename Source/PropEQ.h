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

    double g[N_EQ];

    double p0[N_EQ];
    double p1[N_EQ];
    double p2[N_EQ];
    double q0[N_EQ];
    double q1[N_EQ];
    double q2[N_EQ];

    float y0, y1, y2 = 0.0f;
    float x0, x1, x2 = 0.0f;

    float centerFreqs[N_EQ] = { 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
};

#pragma once
