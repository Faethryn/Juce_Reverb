/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& ) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;



    void drawLabel(juce::Graphics& g, juce::Label& label) override;
};





//==============================================================================

struct RotarySliderWithLabels : juce::Slider
{

    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
        juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;

    juce::Rectangle<int> getSliderBounds() const ;

    int GetTextHeight() const { return 14; }

    juce::String GetDisplayString() const ;

private:

    LookAndFeel lnf;

    juce::RangedAudioParameter* param;
    juce::String suffix; 
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


    RotarySliderWithLabels roomSizeSlider, dampingSlider, wetSlider, drySlider, widthSlider;

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

    const double radialSize = (1.0 / 5.0);

    const double ToggleSize = (1.0 / 10.0);

    const double convolutionSize = (1.0 / 6.0);

    const double labelHeight = (1.0 / 25.0);

   /* juce::Image cheeseKnobImage = juce::ImageCache::getFromMemory(BinaryData::Cheese_knob_png, BinaryData::Cheese_knob_pngSize);*/

    juce::Colour fontColor = juce::Colours::black;

    std::vector<juce::Component*> GetComps();

    LookAndFeel lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceReverbAudioProcessorEditor)
};
