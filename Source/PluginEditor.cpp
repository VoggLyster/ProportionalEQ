/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProportionalEQAudioProcessorEditor::ProportionalEQAudioProcessorEditor (ProportionalEQAudioProcessor& p, 
    juce::AudioProcessorValueTreeState& _vts)
    : AudioProcessorEditor (&p), audioProcessor (p), vts(_vts)
{
    juce::String name = "";
    for (int i = 0; i < N_EQ; i++) {
        addAndMakeVisible(gainSliders[i]);
        name = "g" + juce::String(i);
        gainAttachments[i].reset(new SliderAttachment(vts, name, gainSliders[i]));
        gainSliders[i].setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    }

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
    float w = getWidth() / N_EQ;
    float h = getHeight();

    for (int i = 0; i < N_EQ; i++) {
        gainSliders[i].setBounds(i * w, 0, w, h);
    }
}

void ProportionalEQAudioProcessorEditor::timerCallback() {
    if (audioProcessor.nextFFTBlockReady)
    {
        audioProcessor.drawNextFrameOfSpectrum();
        audioProcessor.nextFFTBlockReady = false;
        repaint();

    }
}
