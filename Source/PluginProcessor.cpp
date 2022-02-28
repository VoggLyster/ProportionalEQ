/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProportionalEQAudioProcessor::ProportionalEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    forwardFFT(fftOrder),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann), 
    parameters(*this, nullptr, juce::Identifier("ProportionalEQ"), createParameterLayout())
#endif
{
    juce::String name = "";
    for (int i = 0; i < N_EQ; i++) {
        name = "g" + juce::String(i);
        gainParameters[i] = parameters.getRawParameterValue(name);
        parameters.addParameterListener(name, this);
    }
}

ProportionalEQAudioProcessor::~ProportionalEQAudioProcessor()
{
}

//==============================================================================
const juce::String ProportionalEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProportionalEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProportionalEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProportionalEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProportionalEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProportionalEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProportionalEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProportionalEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ProportionalEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void ProportionalEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ProportionalEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    propEQs[0] = std::make_unique<PropEQ>(sampleRate);
    propEQs[1] = std::make_unique<PropEQ>(sampleRate);
}

void ProportionalEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProportionalEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ProportionalEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int n = 0; n < buffer.getNumSamples(); n++) {
            channelData[n] = random.nextFloat() * 0.25f - 0.125f;
        }

        for (int n = 0; n < buffer.getNumSamples(); n++) {
            channelData[n] = propEQs[channel]->process(channelData[n]);
        }
    }
    auto* channelData = buffer.getReadPointer(0);
    for (int n = 0; n < buffer.getNumSamples(); n++) {
        pushNextSampleIntoFifo(channelData[n]);
    }
}

void ProportionalEQAudioProcessor::pushNextSampleIntoFifo(float sample) {
    if (fifoIndex == fftSize) {
        if (!nextFFTBlockReady) {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[fifoIndex++] = sample;
}

void ProportionalEQAudioProcessor::drawNextFrameOfSpectrum() {
    window.multiplyWithWindowingTable(fftData, fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);
    auto mindB = -100.0f;
    auto maxdB = 0.0f;
    for (int i = 0; i < scopeSize; i++) {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float) i / (float) 
            scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)
            fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels
        (fftData[fftDataIndex]) - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f);
        scopeData[i] = level;
    }
}

void ProportionalEQAudioProcessor::timerCallback() {
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout ProportionalEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;

    juce::String name = "";
    for (int i = 0; i < N_EQ; i++) {
        name = "g" + juce::String(i);
        params.add(std::make_unique<juce::AudioParameterFloat>(name, name, 0.1f, 2.0f, 1.0f));
    }

    return params;
}

void ProportionalEQAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    float gains[N_EQ];

    for (int i = 0; i < N_EQ; i++)
    {
        gains[i] = *gainParameters[i];
    }

    for (int i = 0; i < 2; i++)
    {
        propEQs[i]->setGainVector(gains);
    }
}

//==============================================================================
bool ProportionalEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProportionalEQAudioProcessor::createEditor()
{
    return new ProportionalEQAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ProportionalEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ProportionalEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProportionalEQAudioProcessor();
}
