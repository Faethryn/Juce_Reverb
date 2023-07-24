/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


struct CustomHorizontalSlider : juce::Slider
{

    CustomHorizontalSlider() : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxLeft)
    {
        
    }
};

//==============================================================================
/**
*/
class JuceReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JuceReverbAudioProcessorEditor (JuceReverbAudioProcessor&);
    ~JuceReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JuceReverbAudioProcessor& audioProcessor;


    juce::TextButton loadBtn;

    juce::ToggleButton reverbToggle;

    juce::ToggleButton convolutionToggle;

    juce::Label reverbLabel;

    juce::Label convolutionLabel;


    CustomHorizontalSlider roomSizeSlider, dampingSlider, wetSlider, drySlider, widthSlider;

    juce::Label roomSizeLabel, dampingLabel, wetLabel, dryLabel, widthLabel;



    using APVTS = juce::AudioProcessorValueTreeState;

    using SliderAttachment = APVTS::SliderAttachment;

    SliderAttachment roomsizeSliderAttachment,
          dampingSliderAttachment,
          wetSliderAttachment,
          drySliderAttachment,
          widthSliderAttachment;

    using ToggleAttachment = APVTS::ButtonAttachment;

    ToggleAttachment convolutionToggleAttachment, reverbToggleAttachment;

    const double frequencyResponseHeightRatio = (1.0 / 5.0);



    std::vector<juce::Component*> GetComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceReverbAudioProcessorEditor)
};
