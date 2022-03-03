/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class ProportionalEQAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    ProportionalEQAudioProcessorEditor (ProportionalEQAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~ProportionalEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void drawFrame(juce::Graphics& g);
    void timerCallback() override;

    juce::LookAndFeel_V4 laf;

    juce::AudioProcessorValueTreeState& vts;
    ProportionalEQAudioProcessor& audioProcessor;

    juce::Slider gainSliders[N_EQ];
    std::unique_ptr<SliderAttachment> gainAttachments[N_EQ];
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProportionalEQAudioProcessorEditor)
};
