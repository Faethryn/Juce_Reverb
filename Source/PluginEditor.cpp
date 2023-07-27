/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
    {

    auto bounds = juce::Rectangle<float>(x, y, width, height);

    g.setColour(juce::Colour(203u, 16u, 30u));

    //g.fillEllipse(bounds);

    
    

    auto center = bounds.getCentre();

    auto size = bounds.getHeight();
    if (size > bounds.getWidth())
    {
        size = bounds.getWidth();
    }

   /* juce::Path p;

    juce::Rectangle<float> r;

    r.setLeft(center.getX() - 2);
    r.setRight(center.getX() + 2);

    r.setTop(bounds.getY());
    r.setBottom(center.getY());

    p.addRectangle(r);*/

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

  /*  p.applyTransform(juce::AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.setColour(juce::Colour(232u, 199u, 66u));*/

    //g.fillPath(p);


    juce::DrawableImage  knobImage ;

    knobImage.setImage(juce::ImageCache::getFromMemory(BinaryData::Cheese_knobV2_png, BinaryData::Cheese_knobV2_pngSize));

    knobImage.setBounds(center.getX() - (size / 2.0), center.getY() - (size / 2.0), size, size);

    bounds.setBounds(center.getX() - (size / 2.0), center.getY() - (size / 2.0), size, size);
   
   // knobImage.draw(g,1.f, juce::AffineTransform::identity);

    
    g.setOpacity(1.0f);
   // g.drawImageTransformed(knobImage, juce::AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()), false);
    g.addTransform(juce::AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
    g.drawImage(knobImage.getImage(), bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, knobImage.getImage().getWidth(), knobImage.getImage().getHeight(), false);
   
  

    }

void LookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    
    
        Path powerButton;

        auto bounds = toggleButton.getLocalBounds();

        auto centre = bounds.getCentre();

        auto size = bounds.getHeight();


        if (bounds.getWidth() < bounds.getHeight())
        {
            size = bounds.getWidth();
        }
        bounds.setWidth(size);
        bounds.setHeight(size);

     
        juce::DrawableImage  toggleImage;

        if (toggleButton.getToggleState())
        {

       toggleImage.setImage(juce::ImageCache::getFromMemory(BinaryData::Cheese_ON_png, BinaryData::Cheese_ON_pngSize));
     
        }
        else 
        {
            toggleImage.setImage(juce::ImageCache::getFromMemory(BinaryData::Cheese_OFF_png, BinaryData::Cheese_OFF_pngSize));

        }

        toggleImage.setBounds(bounds);

    
        g.drawImage(toggleImage.getImage(), bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, toggleImage.getImage().getWidth(), toggleImage.getImage().getHeight(), false);
      
    
   
}


void LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) 
{
    // Set the text color for the label

    
   
    label.setColour(juce::Label::textColourId, juce::Colours::black);
    // Call the base class to draw the label with the updated color
    LookAndFeel_V4::drawLabel(g, label);
}

//==============================================================================



void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    auto startAng = juce::degreesToRadians(180.f + 45.f);
    auto endAng = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants<float>::twoPi;

    auto range = getRange();
    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(), sliderBounds.getHeight(), 
        juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAng, endAng, *this);


}





juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    return getLocalBounds();
}


//==============================================================================
JuceReverbAudioProcessorEditor::JuceReverbAudioProcessorEditor(JuceReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    roomSizeSlider(*audioProcessor.apvts.getParameter("RoomSize"), ""),
    dampingSlider(*audioProcessor.apvts.getParameter("Damping"), ""),
    wetSlider(*audioProcessor.apvts.getParameter("WetLevel"), ""),
    drySlider(*audioProcessor.apvts.getParameter("DryLevel"), ""),
    widthSlider(*audioProcessor.apvts.getParameter("Width"), ""),
    roomsizeSliderAttachment(audioProcessor.apvts, "RoomSize", roomSizeSlider),
    dampingSliderAttachment(audioProcessor.apvts, "Damping", dampingSlider),
    wetSliderAttachment(audioProcessor.apvts, "WetLevel", wetSlider),
    drySliderAttachment(audioProcessor.apvts, "DryLevel", drySlider),
    widthSliderAttachment(audioProcessor.apvts, "Width", widthSlider),
    reverbToggleAttachment(audioProcessor.apvts, "ReverbToggle", reverbToggle),
    convolutionToggleAttachment(audioProcessor.apvts, "ConvolutionToggle", convolutionToggle)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(loadBtn);
    addAndMakeVisible(reverbToggle);
    addAndMakeVisible(convolutionToggle);

    reverbToggle.setLookAndFeel(&lnf);
    convolutionToggle.setLookAndFeel(&lnf);
   
    reverbToggle.onClick = [this] 
    {
        if (reverbToggle.getToggleState() == true && convolutionToggle.getToggleState() == true)
        {
            convolutionToggle.setToggleState(false, juce::sendNotification);

        }
    };

    convolutionToggle.onClick = [this]
    {
        if (convolutionToggle.getToggleState() == true && reverbToggle.getToggleState() == true)
        {
           reverbToggle.setToggleState(false, juce::sendNotification);

        }
    };
    


    for (auto* comp : GetComps())
    {
        addAndMakeVisible(comp);
    }

    loadBtn.setButtonText("Load IR");
    loadBtn.onClick = [this]()
    {
        audioProcessor.LoadButtonPressed();

    };
    convolutionLabel.setText("Convolution Toggle",juce::dontSendNotification);
    reverbLabel.setText("Reverb Toggle", juce::dontSendNotification);

    convolutionLabel.setJustificationType(juce::Justification::centredLeft);
    reverbLabel.setJustificationType(juce::Justification::centredLeft);

    roomSizeLabel.setText("Roomsize", juce::dontSendNotification);
       
       dampingLabel.setText("Damping", juce::dontSendNotification);
       
       wetLabel.setText("Wet level", juce::dontSendNotification);
       
       dryLabel.setText("Dry level", juce::dontSendNotification);
       
       widthLabel.setText("Width", juce::dontSendNotification);

    roomSizeLabel.setJustificationType(juce::Justification::centredTop);

    dampingLabel.setJustificationType(juce::Justification::centredTop);
    wetLabel.setJustificationType(juce::Justification::centredTop);
    dryLabel.setJustificationType(juce::Justification::centredTop);
    widthLabel.setJustificationType(juce::Justification::centredTop);


    roomSizeLabel.setColour(juce::Label::textColourId, fontColor);
  

    setSize (500, 500);
}

JuceReverbAudioProcessorEditor::~JuceReverbAudioProcessorEditor()
{
    reverbToggle.setLookAndFeel(nullptr);
    convolutionToggle.setLookAndFeel(nullptr);
}

//==============================================================================
void JuceReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    juce::DrawableImage  cheeseImage;

    cheeseImage.setImage(juce::ImageCache::getFromMemory(BinaryData::Wood_Board_png, BinaryData::Wood_Board_pngSize));


    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * frequencyResponseHeightRatio);

    cheeseImage.setBounds(bounds);

    g.drawImage(cheeseImage.getImage(), bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, cheeseImage.getImage().getWidth(), cheeseImage.getImage().getHeight(), false);


    auto w = responseArea.getWidth();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i{ 0 }; i < w; ++i)
    {
        double mag = 1.0f;
        auto freq = juce::mapToLog10(double(i) / double(w), 20.0, 20000.0);
        mags[i] = juce::Decibels::gainToDecibels(mag);
    }

    juce::Path frequencyCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();

    auto map = [outputMin, outputMax](double input)
    {
        return juce::jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    frequencyCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); ++i)
    {
        frequencyCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }
    
    g.setColour(juce::Colours::white);
    g.strokePath(frequencyCurve, juce::PathStrokeType(2.f));

}

void JuceReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * frequencyResponseHeightRatio);

   
    auto convolutionArea = bounds.removeFromBottom(bounds.getHeight() * convolutionSize);

    

    auto reverbToggleArea = bounds.removeFromTop(getLocalBounds().getHeight() * ToggleSize);


    auto reverbToggleButtonArea = reverbToggleArea.removeFromLeft(reverbToggleArea.getHeight());
    
    reverbToggle.setBounds(reverbToggleButtonArea);

    auto topRowArea = bounds.removeFromTop(bounds.getHeight() / 2.0);



    
  
    

    

    reverbLabel.setBounds(reverbToggleArea);
    reverbLabel.setColour(juce::Label::textColourId,fontColor);

   

    //label setting

    roomSizeLabel.setColour(juce::Label::textColourId, fontColor);
    dampingLabel.setColour(juce::Label::textColourId, fontColor);
    dryLabel.setColour(juce::Label::textColourId, fontColor);
    wetLabel.setColour(juce::Label::textColourId, fontColor);
    widthLabel.setColour(juce::Label::textColourId, fontColor);

  //  roomSizeLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 10.0)));

    roomSizeSlider.setBounds(topRowArea.removeFromLeft( topRowArea.getWidth() /3.0));

    auto labelHeightFirstRow = roomSizeSlider.getBounds().getY() /*- (getLocalBounds().getHeight() * labelHeight)*/ /*- (roomSizeSlider.getBounds().getHeight() /2)*/;

    roomSizeLabel.setBounds(roomSizeSlider.getBounds().getX(), (labelHeightFirstRow), roomSizeSlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);
    
  

  //  dampingLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 8.0)));

    dampingSlider.setBounds(topRowArea.removeFromLeft(topRowArea.getWidth() / 2.0));
    dampingLabel.setBounds(dampingSlider.getBounds().getX(), (labelHeightFirstRow), dampingSlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);
    


   


  //  dryLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 6.0)));

    drySlider.setBounds(topRowArea);
    dryLabel.setBounds(drySlider.getBounds().getX(), (labelHeightFirstRow), drySlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);

 


  //  wetLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 4.0)));

    wetSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));

    auto labelHeightSecondRow = wetSlider.getBounds().getY() /*- (getLocalBounds().getHeight() * labelHeight)*/ /*- (wetSlider.getBounds().getHeight() / 2)*/;


    wetLabel.setBounds(wetSlider.getBounds().getX(), (labelHeightSecondRow), wetSlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);

  //  widthLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 2.0)));
  



    widthSlider.setBounds(bounds);
    widthLabel.setBounds(widthSlider.getBounds().getX(), (labelHeightSecondRow), widthSlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);


    auto   convolutionButtonBounds = convolutionArea.removeFromRight(convolutionArea.getWidth() / 2.0);

    loadBtn.setBounds(convolutionButtonBounds);

    auto convolutionToggleBounds = convolutionArea.removeFromLeft(getLocalBounds().getHeight() * ToggleSize);

    convolutionToggle.setBounds(convolutionToggleBounds);



    convolutionLabel.setBounds(convolutionArea);

   

    convolutionLabel.setColour(juce::Label::textColourId, fontColor);




}


std::vector<juce::Component*> JuceReverbAudioProcessorEditor::GetComps()
{
    return
    {
        &roomSizeSlider,
        &dampingSlider,
        &wetSlider,
        &drySlider,
        &widthSlider,
        &convolutionLabel,
        &reverbLabel,
        &roomSizeLabel,
        &dampingLabel, 
        &wetLabel, 
        &dryLabel, 
        &widthLabel

    };
}

