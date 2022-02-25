/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProportionalEQAudioProcessorEditor::ProportionalEQAudioProcessorEditor (ProportionalEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);
    setResizable(true, true);
    startTimerHz(30);
}

ProportionalEQAudioProcessorEditor::~ProportionalEQAudioProcessorEditor()
{
}

//==============================================================================
void ProportionalEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    drawFrame(g);

}

void ProportionalEQAudioProcessorEditor::drawFrame(juce::Graphics& g)
{
    for (int i = 1; i < scopeSize; ++i)
    {
        auto width = getLocalBounds().getWidth();
        auto height = getLocalBounds().getHeight();

        g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap(audioProcessor.scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                      (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
                              juce::jmap(audioProcessor.scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
    }
}

void ProportionalEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void ProportionalEQAudioProcessorEditor::timerCallback() {
    if (audioProcessor.nextFFTBlockReady)
    {
        audioProcessor.drawNextFrameOfSpectrum();
        audioProcessor.nextFFTBlockReady = false;
        repaint();

    }
}
