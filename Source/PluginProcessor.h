/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SynthAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    SynthAudioProcessor();
    ~SynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    enum class Waveform { Sine, Sawtooth, Square, Noise };

    void setWaveform(Waveform newWaveform)
    {
        selectedWaveform = newWaveform;
        for (int i = 0; i < MaxPolyphony; ++i)
        {
            voices[i].oscillator.setWaveform(selectedWaveform);
        }
    }

    Waveform getSelectedWaveform() const
    {
        return selectedWaveform;
    }

private:

    Waveform selectedWaveform = Waveform::Sine;

    class Dynamic_Oscillator : public juce::dsp::Oscillator<float>
    {
    public:
        Dynamic_Oscillator() : juce::dsp::Oscillator<float>([](float x) { return std::sin(x); }) {}

        void setWaveform(SynthAudioProcessor::Waveform waveform)
        {
            switch (waveform)
            {
            case SynthAudioProcessor::Waveform::Sine:
                initialise([](float x) { return std::sin(x); });
                break;
            case SynthAudioProcessor::Waveform::Sawtooth:
                // Update the waveform function for Sawtooth
                initialise([](float x) { return x / juce::MathConstants<float>::pi; });
                break;
            case SynthAudioProcessor::Waveform::Square:
                // Update the waveform function for Square
                initialise([](float x) { return (x < 0.0f) ? -1.0f : 1.0f; });
                break;
            case SynthAudioProcessor::Waveform::Noise:
                // Update the waveform function for Noise
                initialise([](float) { return juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f; });
                break;
            }
        }
    };

    //juce::dsp::Oscillator<float> osc{ [](float x) { return std::sin(x); } };

    struct Voice
    {
        Dynamic_Oscillator oscillator;
        juce::dsp::Gain<float> gain;
        int noteNumber = -1; // The note number associated with this voice
    };

    // Declare an array to manage active voices
    static constexpr int MaxPolyphony = 16;
    std::array<Voice, MaxPolyphony> voices;


    //SineOscillator osc;
    juce::dsp::Gain<float> gain;
    std::map<int, float> activeNotes;



    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessor)
};
