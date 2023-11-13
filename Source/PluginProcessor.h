/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const int MaxUnison = 16;
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
            for (auto& osc : voices[i].oscillator) {
                osc.setWaveform(selectedWaveform);
            }
        }
    }

    Waveform getSelectedWaveform() const
    {
        return selectedWaveform;
    }

    void setAttackTime(float newValue) {
        for (auto& voice : voices) {
            voice.voiceEnvelope.setParameters({ newValue, voice.voiceEnvelope.getParameters().decay, voice.voiceEnvelope.getParameters().sustain, voice.voiceEnvelope.getParameters().release });
        }
    }
    void setDecayTime(float newValue) {
        for (auto& voice : voices) {
            voice.voiceEnvelope.setParameters({ voice.voiceEnvelope.getParameters().attack, newValue, voice.voiceEnvelope.getParameters().sustain, voice.voiceEnvelope.getParameters().release });
        }
    }
    void setSustainLevel(float newValue) {
        for (auto& voice : voices) {
            voice.voiceEnvelope.setParameters({ voice.voiceEnvelope.getParameters().attack, voice.voiceEnvelope.getParameters().decay, newValue, voice.voiceEnvelope.getParameters().release });
        }
    }
    void setReleaseTime(float newValue) {
        for (auto& voice : voices) {
            voice.voiceEnvelope.setParameters({ voice.voiceEnvelope.getParameters().attack, voice.voiceEnvelope.getParameters().decay, voice.voiceEnvelope.getParameters().sustain, newValue });
        }
    }

    juce::AudioProcessorValueTreeState& getParameters()
    {
        return parameters;
    }

    float outputVolume = 0.7f;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
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
                initialise([](float x) { return x / juce::MathConstants<float>::pi; });
                break;
            case SynthAudioProcessor::Waveform::Square:
                initialise([](float x) { return (x < 0.0f) ? -1.0f : 1.0f; });
                break;
            case SynthAudioProcessor::Waveform::Noise:
                initialise([](float) { return juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f; });
                break;
            }
        }
    };

    //juce::dsp::Oscillator<float> osc{ [](float x) { return std::sin(x); } };

    struct Voice
    {
        Dynamic_Oscillator oscillator[MaxUnison];  // max unison is 16
        juce::dsp::Gain<float> gain;
        int noteNumber = -1;
        bool isActive;
        unsigned int activationOrder = 0;

        juce::ADSR voiceEnvelope;

        Voice::Voice() : noteNumber(-1)
        {
            voiceEnvelope.setParameters(juce::ADSR::Parameters{
                0.01f, 0.0f, 1.0f, 0.01f }
            );
            isActive = false;

        }
    };

    static constexpr int MaxPolyphony = 64;
    std::array<Voice, MaxPolyphony> voices;

    

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessor)
};
