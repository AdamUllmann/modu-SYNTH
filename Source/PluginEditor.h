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
class SynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SynthAudioProcessorEditor(SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;


    void sliderValueChanged(juce::Slider* slider);
    juce::String SynthAudioProcessorEditor::getWaveformName(int waveformValue);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SynthAudioProcessor& audioProcessor;
    juce::ToggleButton waveformToggleButton;
    juce::Label waveformLabel;
    
    juce::ToggleButton waveformToggleButton2;
    juce::Label waveformLabel2;

    juce::ToggleButton waveformToggleButton3;
    juce::Label waveformLabel3;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> waveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> waveformAttachment2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> waveformAttachment3;

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

    void setupSliderWithLabel(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Slider::SliderStyle style);

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};
