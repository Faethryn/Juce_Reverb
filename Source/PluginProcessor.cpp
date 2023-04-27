/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
JuceReverbAudioProcessor::JuceReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

JuceReverbAudioProcessor::~JuceReverbAudioProcessor()
{
}

//==============================================================================
const juce::String JuceReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JuceReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JuceReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JuceReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JuceReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JuceReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JuceReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JuceReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JuceReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void JuceReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JuceReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = getSampleRate();
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2; // stereo

    reverbChain.prepare(spec);

    auto chainSettings = getChainSettings(apvts);
    UpdateReverbParams(chainSettings);




   

  
    





}

void JuceReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JuceReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void JuceReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto chainSettings = getChainSettings(apvts);
   
    UpdateReverbParams(chainSettings);

    juce::dsp::AudioBlock<float> inputBlock(buffer);

    // Create AudioBlock objects for the left and right output channels
   

    juce::dsp::ProcessContextReplacing<float> context(inputBlock);

    reverbChain.process(context);




    
}

//==============================================================================
bool JuceReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JuceReverbAudioProcessor::createEditor()
{
   // return new JuceReverbAudioProcessorEditor (*this);

    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void JuceReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JuceReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.RoomSize = apvts.getRawParameterValue("RoomSize")->load();
    settings.Damping = apvts.getRawParameterValue("Damping")->load();
    settings.WetLevel = apvts.getRawParameterValue("WetLevel")->load();
    settings.DryLevel = apvts.getRawParameterValue("DryLevel")->load();
    settings.Width = apvts.getRawParameterValue("Width")->load();


    return settings;
}

void JuceReverbAudioProcessor::UpdateReverbParams(const ChainSettings& chainSettings)
{
    auto& reverbFromChain = reverbChain.template get<0>();
    auto reverbParams = reverbFromChain.getParameters();
    reverbParams.roomSize = chainSettings.RoomSize;
    reverbParams.damping = chainSettings.Damping;
    reverbParams.wetLevel = chainSettings.WetLevel;
    reverbParams.dryLevel = chainSettings.DryLevel;
    reverbParams.width = chainSettings.Width;

    reverbFromChain.setParameters(reverbParams);
}

 juce::AudioProcessorValueTreeState::ParameterLayout JuceReverbAudioProcessor::createParameterLayout()
{
     juce::AudioProcessorValueTreeState::ParameterLayout layout;


     layout.add(std::make_unique<juce::AudioParameterFloat>("RoomSize", "RoomSize", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), 0.1f));
     layout.add(std::make_unique<juce::AudioParameterFloat>("Damping", "Damping", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), 0.1f));
     layout.add(std::make_unique<juce::AudioParameterFloat>("WetLevel", "WetLevel", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), 0.1f));
     layout.add(std::make_unique<juce::AudioParameterFloat>("DryLevel", "DryLevel", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), 0.1f));
     layout.add(std::make_unique<juce::AudioParameterFloat>("Width", "Width", juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), 0.1f));


     return layout;
}





//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceReverbAudioProcessor(); 
}
