/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    addAndMakeVisible(waveformToggleButton);
    waveformToggleButton.setButtonText("Sine");

    waveformToggleButton.onClick = [this]
    {
        switch (audioProcessor.getSelectedWaveform())
        {
        case SynthAudioProcessor::Waveform::Sine:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sawtooth);
            waveformToggleButton.setButtonText("Sawtooth");
            break;
        case SynthAudioProcessor::Waveform::Sawtooth:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Square);
            waveformToggleButton.setButtonText("Square");
            break;
        case SynthAudioProcessor::Waveform::Square:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Noise);
            waveformToggleButton.setButtonText("Noise");
            break;
        case SynthAudioProcessor::Waveform::Noise:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sine);
            waveformToggleButton.setButtonText("Sine");
            break;
        }
    };

    addAndMakeVisible(waveformLabel);
    waveformLabel.setText("Waveform:", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveformToggleButton, true);

}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor()
{
}

//==============================================================================
void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(30.0f);

    juce::Font modernFont("Sans-Sarif", 30.0f, juce::Font::bold);
    g.setFont(modernFont);

    juce::Rectangle<int> bounds = getLocalBounds();

    juce::Colour modernColor(0x3355FF50);
    
    g.setColour(modernColor);
    g.fillRect(bounds);

    g.setColour(juce::Colours::white);
    juce::Rectangle<float> roundedRect = bounds.toFloat().reduced(20.0f);
    g.drawRoundedRectangle(roundedRect, 15.0f, 2.0f);

    g.setColour(juce::Colours::white);
    g.drawText("Modu-SYNTH", bounds, juce::Justification::centred, true);
}

void SynthAudioProcessorEditor::resized()
{
    const int buttonWidth = 100;
    const int buttonHeight = 30;

    waveformToggleButton.setBounds(120, 190, buttonWidth, buttonHeight);

    waveformLabel.setBounds(120, 170, getWidth() - 20, 20);

    waveformToggleButton.setToggleState(false, juce::NotificationType::dontSendNotification);

}
