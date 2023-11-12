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

    setupSliderWithLabel(attackSlider, attackLabel, "Attack", juce::Slider::LinearVertical);
    setupSliderWithLabel(decaySlider, decayLabel, "Decay", juce::Slider::LinearVertical);
    setupSliderWithLabel(sustainSlider, sustainLabel, "Sustain", juce::Slider::LinearVertical);
    setupSliderWithLabel(releaseSlider, releaseLabel, "Release", juce::Slider::LinearVertical);

    attackSlider.setRange(0.001, 5.0, 0.01);
    decaySlider.setRange(0.0, 5.0, 0.01);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    releaseSlider.setRange(0.001, 5.0, 0.01);

    attackSlider.onValueChange = [this] { audioProcessor.setAttackTime(attackSlider.getValue()); };
    decaySlider.onValueChange = [this] { audioProcessor.setDecayTime(decaySlider.getValue()); };
    sustainSlider.onValueChange = [this] { audioProcessor.setSustainLevel(sustainSlider.getValue()); };
    releaseSlider.onValueChange = [this] { audioProcessor.setReleaseTime(releaseSlider.getValue()); };

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

    int sliderWidth = 50;
    int sliderHeight = 100;
    int sliderStartX = 140;
    int sliderSpacing = 60;

    attackSlider.setBounds(sliderStartX, 190, sliderWidth, sliderHeight);
    decaySlider.setBounds(sliderStartX + sliderSpacing, 190, sliderWidth, sliderHeight);
    sustainSlider.setBounds(sliderStartX + 2 * sliderSpacing, 190, sliderWidth, sliderHeight);
    releaseSlider.setBounds(sliderStartX + 3 * sliderSpacing, 190, sliderWidth, sliderHeight);

}

//=======================================================================================================

void SynthAudioProcessorEditor::setupSliderWithLabel(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Slider::SliderStyle style)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
}

void SynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &attackSlider)
    {
        audioProcessor.setAttackTime(attackSlider.getValue());
    }
    else if (slider == &decaySlider)
    {
        audioProcessor.setDecayTime(decaySlider.getValue());
    }
    else if (slider == &sustainSlider)
    {
        audioProcessor.setSustainLevel(sustainSlider.getValue());
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setReleaseTime(releaseSlider.getValue());
    }
}