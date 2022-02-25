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
class ProportionalEQAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    ProportionalEQAudioProcessorEditor (ProportionalEQAudioProcessor&);
    ~ProportionalEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void drawFrame(juce::Graphics& g);
    void timerCallback() override;

    ProportionalEQAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProportionalEQAudioProcessorEditor)
};
