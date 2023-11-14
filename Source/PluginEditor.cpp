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
    setSize(600, 500);

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
    waveform2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getParameters(), "waveform", waveformToggleButton2);
    waveform3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getParameters(), "waveform", waveformToggleButton3);

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


    unisonSlider1.setRange(1, 16, 1);
    unisonSlider1.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    unisonSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 25, 15);
    addAndMakeVisible(unisonSlider1);

    detuneSlider1.setRange(0.0, 5.0, 0.01);
    detuneSlider1.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    detuneSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
    addAndMakeVisible(detuneSlider1);

    unisonSlider2.setRange(1, 16, 1);
    unisonSlider2.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    unisonSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 25, 15);
    addAndMakeVisible(unisonSlider2);

    detuneSlider2.setRange(0.0, 5.0, 0.01);
    detuneSlider2.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    detuneSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
    addAndMakeVisible(detuneSlider2);

    unisonSlider3.setRange(1, 16, 1);
    unisonSlider3.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    unisonSlider3.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 25, 15);
    addAndMakeVisible(unisonSlider3);

    detuneSlider3.setRange(0.0, 5.0, 0.01);
    detuneSlider3.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    detuneSlider3.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 15);
    addAndMakeVisible(detuneSlider3);

    unisonSlider1.onValueChange = [this] { 
        audioProcessor.setUnisonCount(0, unisonSlider1.getValue());
        //unisonValueLabel1.setText(juce::String(unisonSlider1.getValue()), juce::dontSendNotification);
    };
    detuneSlider1.onValueChange = [this] { 
        audioProcessor.setDetuneAmount(0, detuneSlider1.getValue());
        //detuneValueLabel1.setText(juce::String(unisonSlider1.getValue()), juce::dontSendNotification);
    };

    unisonSlider2.onValueChange = [this] { 
        audioProcessor.setUnisonCount(1, unisonSlider2.getValue());
        //unisonValueLabel2.setText(juce::String(unisonSlider1.getValue()), juce::dontSendNotification);
    };
    detuneSlider2.onValueChange = [this] { 
        audioProcessor.setDetuneAmount(1, detuneSlider2.getValue()); 
        //detuneValueLabel2.setText(juce::String(unisonSlider1.getValue()), juce::dontSendNotification);
    };

    unisonSlider3.onValueChange = [this] { 
        audioProcessor.setUnisonCount(2, unisonSlider3.getValue()); 
        //unisonValueLabel3.setText(juce::String(unisonSlider1.getValue()), juce::dontSendNotification);
    };
    detuneSlider3.onValueChange = [this] { 
        audioProcessor.setDetuneAmount(2, detuneSlider3.getValue()); 
        //detuneValueLabel3.setText(juce::String(unisonSlider1.getValue()), juce::dontSendNotification);
    };

    unison1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "unison1", unisonSlider1);
    detune1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "detune1", detuneSlider1);
    unison2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "unison2", unisonSlider2);
    detune2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "detune2", detuneSlider2);
    unison3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "unison3", unisonSlider3);
    detune3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getParameters(), "detune3", detuneSlider3);

    unisonLabel.setText("Unison", juce::dontSendNotification); 
    detuneLabel.setText("Detune", juce::dontSendNotification);
    addAndMakeVisible(unisonLabel);
    addAndMakeVisible(detuneLabel);

    //unisonValueLabel1.setText("1", juce::dontSendNotification);
    //addAndMakeVisible(unisonValueLabel1);

    //detuneValueLabel1.setText("0.0", juce::dontSendNotification); 
    //addAndMakeVisible(detuneValueLabel1);

    //unisonValueLabel2.setText("1", juce::dontSendNotification); 
    //addAndMakeVisible(unisonValueLabel2);

    //detuneValueLabel2.setText("0.0", juce::dontSendNotification); 
    //addAndMakeVisible(detuneValueLabel2);

    //unisonValueLabel3.setText("1", juce::dontSendNotification); 
    //addAndMakeVisible(unisonValueLabel3);

    //detuneValueLabel3.setText("0.0", juce::dontSendNotification); 
    //addAndMakeVisible(detuneValueLabel3);

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

    juce::Font modernFont("Sans-Sarif", 45.0f, juce::Font::bold);
    g.setFont(modernFont);

    juce::Rectangle<int> bounds = getLocalBounds();

    juce::Colour modernColor(0x3355FF50);
    
    g.setColour(modernColor);
    g.fillRect(bounds);

    g.setColour(juce::Colours::white);
    juce::Rectangle<float> roundedRect = bounds.toFloat().reduced(20.0f);
    g.drawRoundedRectangle(roundedRect, 15.0f, 2.0f);

    juce::Rectangle<int> title = { 600, 550 };
    g.setColour(juce::Colours::white);
    g.drawText("Modu-SYNTH", title, juce::Justification::centred, true);
}

void SynthAudioProcessorEditor::resized()
{
    const int knobSize = 60;
    const int labelHeight = 20;
    const int spaceBetween = 30;

    unisonLabel.setBounds(140 + knobSize + spaceBetween + 35, 30, 100, 20);
    detuneLabel.setBounds(180 + knobSize + spaceBetween + 55, 30, 100, 20);

    waveformToggleButton.setBounds(150, 60, 100, 30);
    waveformLabel.setBounds(150, 30, getWidth() - 20, labelHeight);
    //unisonValueLabel1.setBounds(140 + knobSize + spaceBetween, 70, 100, 20);
    //detuneValueLabel1.setBounds(180 + knobSize + spaceBetween, 70, 100, 20);
    unisonSlider1.setBounds(160 + knobSize + spaceBetween + 10, 50, knobSize, knobSize);
    detuneSlider1.setBounds(200 + knobSize + spaceBetween + 30, 50, knobSize, knobSize);

    waveformToggleButton2.setBounds(150, 120, 100, 30);
    waveformLabel2.setBounds(150, 70, getWidth() - 20, labelHeight);
    unisonSlider2.setBounds(160 + knobSize + spaceBetween + 10, 110, knobSize, knobSize);
    detuneSlider2.setBounds(200 + knobSize + spaceBetween + 30, 110, knobSize, knobSize);

    waveformToggleButton3.setBounds(150, 180, 100, 30);
    waveformLabel3.setBounds(150, 110, getWidth() - 20, labelHeight);
    unisonSlider3.setBounds(160 + knobSize + spaceBetween + 10, 170, knobSize, knobSize);
    detuneSlider3.setBounds(200 + knobSize + spaceBetween + 30, 170, knobSize, knobSize);

    //waveformToggleButton.setToggleState(false, juce::NotificationType::dontSendNotification);

    int sliderWidth = 50;
    int sliderHeight = 100;
    int sliderStartX = 185;
    int sliderSpacing = 60;

    attackSlider.setBounds(sliderStartX, 350, sliderWidth, sliderHeight);
    decaySlider.setBounds(sliderStartX + sliderSpacing, 350, sliderWidth, sliderHeight);
    sustainSlider.setBounds(sliderStartX + 2 * sliderSpacing, 350, sliderWidth, sliderHeight);
    releaseSlider.setBounds(sliderStartX + 3 * sliderSpacing, 350, sliderWidth, sliderHeight);

    int sliderDiameter = 50;
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

