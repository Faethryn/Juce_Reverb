/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct ChainSettings
{
    float RoomSize{ 0.1f }, Damping{ 0.1f }, WetLevel{ 1.f }, DryLevel{ 0.0f }, Width{ 0.1f };
};



ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class JuceReverbAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    JuceReverbAudioProcessor();
    ~JuceReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

   



    static   juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    

    // Declare the AudioProcessorValueTreeState instance
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "reverbParameters", createParameterLayout()};

    

    void LoadButtonPressed();
   
private:

    juce::File root, savedFile; 

   std::unique_ptr<juce::FileChooser> fileChooser;

    using ReverbProcessor = juce::dsp::Reverb;
   
        
    ReverbProcessor reverbProcess;

    using ConvolutionProcessor = juce::dsp::Convolution;

    ConvolutionProcessor convProcess;

    using StereoChain = juce::dsp::ProcessorChain<ReverbProcessor>;

    using ConvStereoChain = juce::dsp::ProcessorChain<ConvolutionProcessor>;


    StereoChain reverbChain;

    ConvStereoChain convChain;

    void UpdateReverbParams(const ChainSettings& chainSettings);

    void UpdateToggles(juce::AudioProcessorValueTreeState& apvts);

    void UpdateConvolutionIR(juce::File& irFile);

    void UpdateFiles(juce::String filePath);

    
    

    void LoadingIRFile();


    bool reverbToggle = false;

    bool convToggle = true; 

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceReverbAudioProcessor)
};
