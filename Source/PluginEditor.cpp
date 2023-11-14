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
    setSize(400, 400);

    addAndMakeVisible(waveformToggleButton);
    waveformToggleButton.setButtonText("Sine");

    waveformToggleButton.onClick = [this]
    {
        switch (audioProcessor.getSelectedWaveform(0))
        {
        case SynthAudioProcessor::Waveform::Sine:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sawtooth, 0);
            waveformToggleButton.setButtonText("Sawtooth");
            break;
        case SynthAudioProcessor::Waveform::Sawtooth:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Square, 0);
            waveformToggleButton.setButtonText("Square");
            break;
        case SynthAudioProcessor::Waveform::Square:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Noise, 0);
            waveformToggleButton.setButtonText("Noise");
            break;
        case SynthAudioProcessor::Waveform::Noise:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sine, 0);
            waveformToggleButton.setButtonText("Sine");
            break;
        }
    };
    
    addAndMakeVisible(waveformLabel);
    waveformLabel.setText("Osc 1 Waveform:", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveformToggleButton, true);


    addAndMakeVisible(waveformToggleButton2);
    waveformToggleButton2.setButtonText("Sine");

    waveformToggleButton2.onClick = [this] { 
        switch (audioProcessor.getSelectedWaveform(1))
            {
            case SynthAudioProcessor::Waveform::Sine:
                 audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sawtooth, 1);
                waveformToggleButton2.setButtonText("Sawtooth");
                break;
            case SynthAudioProcessor::Waveform::Sawtooth:
                audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Square, 1);
                waveformToggleButton2.setButtonText("Square");
                break;
            case SynthAudioProcessor::Waveform::Square:
                audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Noise, 1);
                waveformToggleButton2.setButtonText("Noise");
                break;
            case SynthAudioProcessor::Waveform::Noise:
                audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sine, 1);
                waveformToggleButton2.setButtonText("Sine");
                break;
            }
    };

    addAndMakeVisible(waveformLabel2);
    waveformLabel2.setText("Osc 2 Waveform:", juce::dontSendNotification);
    waveformLabel2.attachToComponent(&waveformToggleButton2, true);


    addAndMakeVisible(waveformToggleButton3);
    waveformToggleButton3.setButtonText("Sine");

    waveformToggleButton3.onClick = [this] { 
        switch (audioProcessor.getSelectedWaveform(2))
        {
        case SynthAudioProcessor::Waveform::Sine:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sawtooth, 2);
            waveformToggleButton3.setButtonText("Sawtooth");
            break;
        case SynthAudioProcessor::Waveform::Sawtooth:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Square, 2);
            waveformToggleButton3.setButtonText("Square");
            break;
        case SynthAudioProcessor::Waveform::Square:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Noise, 2);
            waveformToggleButton3.setButtonText("Noise");
            break;
        case SynthAudioProcessor::Waveform::Noise:
            audioProcessor.setWaveform(SynthAudioProcessor::Waveform::Sine, 2);
            waveformToggleButton3.setButtonText("Sine");
            break;
        }
    };

    addAndMakeVisible(waveformLabel3);
    waveformLabel3.setText("Osc 3 Waveform:", juce::dontSendNotification);
    waveformLabel3.attachToComponent(&waveformToggleButton3, true);


    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getParameters(), "waveform", waveformToggleButton);

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

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "release", releaseSlider);

    volumeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(audioProcessor.outputVolume);
    volumeSlider.onValueChange = [this] { audioProcessor.outputVolume = volumeSlider.getValue(); };
    addAndMakeVisible(volumeSlider);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, false);
    addAndMakeVisible(volumeLabel);

    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "volume", volumeSlider);


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

    waveformToggleButton.setBounds(120, 50, buttonWidth, buttonHeight);
    waveformLabel.setBounds(120, 30, getWidth() - 20, 20);

    waveformToggleButton2.setBounds(120, 70, buttonWidth, buttonHeight);
    waveformLabel2.setBounds(120, 50, getWidth() - 20, 20);

    waveformToggleButton3.setBounds(120, 90, buttonWidth, buttonHeight);
    waveformLabel3.setBounds(120, 70, getWidth() - 20, 20);

    //waveformToggleButton.setToggleState(false, juce::NotificationType::dontSendNotification);

    int sliderWidth = 50;
    int sliderHeight = 100;
    int sliderStartX = 85;
    int sliderSpacing = 60;

    attackSlider.setBounds(sliderStartX, 250, sliderWidth, sliderHeight);
    decaySlider.setBounds(sliderStartX + sliderSpacing, 250, sliderWidth, sliderHeight);
    sustainSlider.setBounds(sliderStartX + 2 * sliderSpacing, 250, sliderWidth, sliderHeight);
    releaseSlider.setBounds(sliderStartX + 3 * sliderSpacing, 250, sliderWidth, sliderHeight);

    int sliderDiameter = 50;
    int labelHeight = 20;
    volumeSlider.setBounds(getWidth() - sliderDiameter - 18, 20, sliderDiameter, sliderDiameter);
    volumeLabel.setBounds(getWidth() - sliderDiameter - 18, 20 + sliderDiameter, sliderDiameter, labelHeight);

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