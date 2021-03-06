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
    juce::dsp::ProcessSpec spec = juce::dsp::ProcessSpec();
    spec.maximumBlockSize = 4096;
    spec.numChannels = 1;
    spec.sampleRate = _samplerate;
    samplerate = _samplerate;
    for (int i = 0; i < N_EQ; i++) {
        iir[i] = juce::dsp::IIR::Filter<float>();
        iir[i].prepare(spec);
    }
    setPolesAndZeros();
    for (int i = 0; i < N_EQ; i++) {
        iir[i].reset();
    }
}

void PropEQ::setGainVector(float gainVector[N_EQ])
{
    for (int i = 0; i < N_EQ; i++) {
        g[i] = gainVector[i];
    }
    setPolesAndZeros();
}

float PropEQ::process(float input)
{
    tmpVal = input;
    for (int i = 0; i < N_EQ; i++) {
        tmpVal = iir[i].processSample(tmpVal);
    }
    return tmpVal;
}

void PropEQ::setPolesAndZeros()
{
    iir[0].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(samplerate, centerFreqs[0], Q, g[0]);

    for (int i = 1; i < N_EQ - 1; i++) {
        iir[i].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(samplerate, centerFreqs[i], Q, g[i]);
    }

    iir[N_EQ - 1].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(samplerate, centerFreqs[N_EQ - 1], Q, g[N_EQ - 1]);
}


