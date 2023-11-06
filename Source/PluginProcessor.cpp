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
    )
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
    gain.prepare(spec);

    //osc.setFrequency(440.0f);
    gain.setGainLinear(0.5f);

    for (int i = 0; i < MaxPolyphony; ++i)
    {
        Voice& voice = voices[i];

        // Initialize the oscillator and gain for this voice
        voice.oscillator.prepare(spec); // Use the spec from prepareToPlay
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

    // Process MIDI messages
    juce::MidiBuffer::Iterator it(midiMessages);
    juce::MidiMessage message;
    int samplePosition;

    while (it.getNextEvent(message, samplePosition))
    {

        if (message.isNoteOn())
        {
            int noteNumber = message.getNoteNumber();
            int velocity = message.getVelocity();

            float frequency = 440.0 * pow(2.0, (noteNumber - 69) / 12.0);

            for (int i = 0; i < MaxPolyphony; ++i)
            {
                if (voices[i].noteNumber == -1)
                {
                    Voice& voice = voices[i];
                    voice.noteNumber = noteNumber;
                    voice.oscillator.setFrequency(frequency);
                    voice.gain.setGainLinear(velocity / 127.0f);
                    break;
                }
            }
        }
        else if (message.isNoteOff())
        {
            int noteNumber = message.getNoteNumber();

            for (int i = 0; i < MaxPolyphony; ++i)
            {
                if (voices[i].noteNumber == noteNumber)
                {
                    voices[i].noteNumber = -1;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < MaxPolyphony; ++i)
    {
        Voice& voice = voices[i];

        if (voice.noteNumber != -1)
        {
            //voice.oscillator.setFrequency(voice.oscillator.getFrequency());

            juce::dsp::AudioBlock<float> voiceBlock(audioBlock);
            voice.oscillator.process(juce::dsp::ProcessContextReplacing<float>(voiceBlock));
            voice.gain.process(juce::dsp::ProcessContextReplacing<float>(voiceBlock));
        }
    }

    // Process active notes
/*    audioBlock.clear(); // Clear the audio block

    for (const auto& note : activeNotes)
    {
        osc.setFrequency(note.second); // Set the oscillator frequency for the active note
        osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    }

    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    */
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
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}
