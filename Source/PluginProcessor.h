/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PropEQ.h"

enum
{
    fftOrder = 11,
    fftSize = 1 << fftOrder,
    scopeSize = 512
};

//==============================================================================
/**
*/
class ProportionalEQAudioProcessor  : public juce::AudioProcessor, public juce::Timer,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ProportionalEQAudioProcessor();
    ~ProportionalEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void drawNextFrameOfSpectrum();
    bool nextFFTBlockReady = false;
    float scopeData[scopeSize];

private:
    juce::Random random;
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* gainParameters[N_EQ];

    std::unique_ptr<PropEQ> propEQs[2];
    

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;

    void pushNextSampleIntoFifo(float sample);
    void timerCallback() override;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProportionalEQAudioProcessor)

    // Inherited via Listener
    virtual void parameterChanged(const juce::String& parameterID, float newValue) override;
};
