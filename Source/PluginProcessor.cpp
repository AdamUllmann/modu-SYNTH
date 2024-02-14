/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthAudioProcessor::SynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, "Parameters", createParams())
#endif
{
    

}

SynthAudioProcessor::~SynthAudioProcessor()
{
}

//==============================================================================
const juce::String SynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SynthAudioProcessor::getProgramName(int index)
{
    return {};
}

void SynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    //osc.prepare(spec);
    //gain.prepare(spec);

    //osc.setFrequency(440.0f);
    //gain.setGainLinear(0.5f);

    for (int i = 0; i < MaxPolyphony; ++i)
    {
        Voice& voice = voices[i];
        for (int j = 0; j < 3; j++) {
            for (auto& osc : voice.oscillators[j].oscillator) {
                osc.prepare(spec);
            }
        }
        voice.gain.prepare(spec);
        voice.gain.setGainLinear(0.5f);
    }
}

void SynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; i++) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    juce::dsp::AudioBlock<float> audioBlock{ buffer };

    juce::MidiBuffer::Iterator it(midiMessages);
    juce::MidiMessage message;
    int samplePosition;
    static unsigned int nextActivationOrder = 1;
    static unsigned int nextVoiceIndex = 0;

    buffer.clear();

    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);

    while (it.getNextEvent(message, samplePosition)) {
        if (message.isNoteOn()) {
            int noteNumber = message.getNoteNumber();
            int velocity = message.getVelocity();

            float frequency = juce::MidiMessage::getMidiNoteInHertz(noteNumber);

            Voice* voiceToUse = nullptr;


            for (unsigned int i = 0; i < MaxPolyphony; ++i) {
                auto& voice = voices[(nextVoiceIndex + i) % MaxPolyphony];
                if (!voice.isActive) {
                    voiceToUse = &voice;
                    nextVoiceIndex = (nextVoiceIndex + i + 1) % MaxPolyphony;
                    break;
                }
            }

            if (!voiceToUse) {
                unsigned int oldestOrder = nextActivationOrder;
                for (auto& voice : voices) {
                    if (voice.activationOrder < oldestOrder) {
                        oldestOrder = voice.activationOrder;
                        voiceToUse = &voice;
                        //break;
                    }
                }
                if (voiceToUse) {
                    voiceToUse->voiceEnvelope.reset();
                }
            }

            if (voiceToUse) {
                voiceToUse->activationOrder = nextActivationOrder++;
                voiceToUse->noteNumber = noteNumber;
                voiceToUse->isActive = true;

                float osc1Tune = *parameters.getRawParameterValue("osc1Tune");
                float osc2Tune = *parameters.getRawParameterValue("osc2Tune");
                float osc3Tune = *parameters.getRawParameterValue("osc3Tune");

                for (int i = 0; i < 3; i++) {
                    for (int u = 0; u < voiceToUse->oscillators[i].unison; ++u) {
                        voiceToUse->oscillators[i].oscillator[u].reset();

                     //   float tune = (i == 0) ? osc1Tune : (i == 1) ? osc2Tune : osc3Tune;
                     //   float tunedFrequency = frequency * tune;

                        if (voiceToUse->oscillators[i].unison != 1) {
                            float randomPhase = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * juce::MathConstants<float>::twoPi;
                            voiceToUse->oscillators[i].oscillator[u].setPhase(randomPhase);
                        }
                        float detune = (u - (voiceToUse->oscillators[i].unison / 2)) * voiceToUse->oscillators[i].detune;
                        voiceToUse->oscillators[i].oscillator[u].setFrequency(frequency + detune);
                    }
                }
                voiceToUse->gain.setGainLinear(velocity / 127.0f);
                voiceToUse->voiceEnvelope.noteOn();
            }
        }
        else if (message.isNoteOff()) {
            int noteNumber = message.getNoteNumber();

            for (auto& voice : voices) {
                if (voice.isActive && voice.noteNumber == noteNumber) {
                    voice.voiceEnvelope.noteOff();
                }
            }
        }
    }

    for (int i = 0; i < MaxPolyphony; ++i)
    {
        Voice& voice = voices[i];

        if (voice.isActive)
        {
            tempBuffer.clear();

            juce::dsp::AudioBlock<float> voiceBlock(tempBuffer);

            float osc1Volume = *parameters.getRawParameterValue("osc1Volume");
            float osc2Volume = *parameters.getRawParameterValue("osc2Volume");
            float osc3Volume = *parameters.getRawParameterValue("osc3Volume");


            juce::AudioBuffer<float> oscBuffers[3];

            for (int i = 0; i < 3; ++i) {
                oscBuffers[i].setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
                oscBuffers[i].clear();
            }

            for (int oscIndex = 0; oscIndex < 3; ++oscIndex) {
                auto& oscBuffer = oscBuffers[oscIndex];
                juce::dsp::AudioBlock<float> oscBlock(oscBuffer);

                for (int u = 0; u < voice.oscillators[oscIndex].unison; ++u) {
                    voice.oscillators[oscIndex].oscillator[u].process(juce::dsp::ProcessContextReplacing<float>(oscBlock));
                }

                float volume = (oscIndex == 0) ? osc1Volume : (oscIndex == 1) ? osc2Volume : osc3Volume;
                oscBuffer.applyGain(volume);
            }

            for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel) {
                auto* channelData = tempBuffer.getWritePointer(channel);
                for (int sample = 0; sample < tempBuffer.getNumSamples(); ++sample) {
                    for (int oscIndex = 0; oscIndex < 3; ++oscIndex) {
                        channelData[sample] += oscBuffers[oscIndex].getReadPointer(channel)[sample];
                    }
                }
            }

            voice.gain.process(juce::dsp::ProcessContextReplacing<float>(voiceBlock));
            voice.voiceEnvelope.applyEnvelopeToBuffer(tempBuffer, 0, tempBuffer.getNumSamples());

            for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel) {
                auto* channelData = tempBuffer.getWritePointer(channel);
                for (int sample = 0; sample < tempBuffer.getNumSamples(); ++sample) {
                    channelData[sample] *= outputVolume;
                }
            }

            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                buffer.addFrom(channel, 0, tempBuffer, channel, 0, buffer.getNumSamples(), outputVolume);
            }

            if (!voice.voiceEnvelope.isActive()) {
                voice.isActive = false;
            }
        }
    }
}




//==============================================================================
bool SynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new SynthAudioProcessorEditor(*this);
}

//==============================================================================
void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);

}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SynthAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.001f, 5.0f, 0.01f), 0.001f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(0.001f, 5.0f, 0.01f), 0.001f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("volume", "Volume", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterInt>("unisonCount", "Unison Count", 1, 16, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("detuneAmount", "Detune Amount", 0.0f, 5.0f, 0.01f));

    juce::StringArray waveformChoices = { "Sine", "Sawtooth", "Square", "Noise" };
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc1Waveform", "Oscillator 1 Waveform", waveformChoices, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc2Waveform", "Oscillator 2 Waveform", waveformChoices, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc3Waveform", "Oscillator 3 Waveform", waveformChoices, 0));

    for (int i = 1; i <= 3; ++i) {
        params.push_back(std::make_unique<juce::AudioParameterInt>("unison" + std::to_string(i), "Unison " + std::to_string(i), 1, 16, 1));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("detune" + std::to_string(i), "Detune " + std::to_string(i), 0.0f, 5.0f, 0.0f));
    }

    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc1Volume", "Oscillator 1 Volume", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc2Volume", "Oscillator 2 Volume", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc3Volume", "Oscillator 3 Volume", 0.0f, 1.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc1Tune", "Oscillator 1 Tune", 0.5f, 2.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc2Tune", "Oscillator 2 Tune", 0.5f, 2.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc3Tune", "Oscillator 3 Tune", 0.5f, 2.0f, 1.0f));

    return { params.begin(), params.end() };
}
