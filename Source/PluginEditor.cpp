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

    juce::Path p;

    juce::Rectangle<float> r;

    r.setLeft(center.getX() - 2);
    r.setRight(center.getX() + 2);

    r.setTop(bounds.getY());
    r.setBottom(center.getY());

    p.addRectangle(r);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(juce::AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.setColour(juce::Colour(232u, 199u, 66u));

    //g.fillPath(p);


    juce::DrawableImage  knobImage ;

    knobImage.setImage(juce::ImageCache::getFromMemory(BinaryData::Cheese_knob_png, BinaryData::Cheese_knob_pngSize));

    knobImage.setBounds(x, y, width, height);
   
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

        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

        float ang = 30.f; //30.f;

        size -= 6;

        powerButton.addCentredArc(r.getCentreX(),
            r.getCentreY(),
            size * 0.5,
            size * 0.5,
            0.f,
            degreesToRadians(ang),
            degreesToRadians(360.f - ang),
            true);

        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());

        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

        auto color = toggleButton.getToggleState() ? juce::Colours::crimson : Colours::dimgrey;

        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    
   
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

    roomSizeLabel.setText("Roomsize", juce::dontSendNotification);
       
       dampingLabel.setText("Damping", juce::dontSendNotification);
       
       wetLabel.setText("Wet level", juce::dontSendNotification);
       
       dryLabel.setText("Dry level", juce::dontSendNotification);
       
       widthLabel.setText("Width", juce::dontSendNotification);

    

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

    cheeseImage.setImage(juce::ImageCache::getFromMemory(BinaryData::Kraft_single_png, BinaryData::Kraft_single_pngSize));


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

    auto reverbArea = bounds.removeFromTop(bounds.getHeight() * 0.75);
    bounds.removeFromTop(bounds.getHeight() / 6);
    auto convolutionArea = bounds;



    auto reverbToggleArea = reverbArea;
    reverbToggleArea = reverbToggleArea.removeFromLeft(reverbToggleArea.getWidth() * ToggleSize);
    
    reverbToggle.setBounds(reverbToggleArea.removeFromTop(reverbArea.getHeight() * (1.0 / 6.0)));

     reverbToggleArea = reverbToggle.getBounds();
     auto reverbLabelArea = reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 6.0));
    

    

    reverbLabel.setBounds(reverbLabelArea.removeFromRight(reverbLabelArea.getWidth() * 0.8));
    reverbLabel.setColour(juce::Label::textColourId,fontColor);

    double sliderSize{};

    reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 6.0));

    if (reverbArea.getHeight() < reverbArea.getWidth())
    {
        sliderSize = reverbArea.getWidth() * radialSize;

    }
    else
    {
        sliderSize = reverbArea.getHeight() * radialSize;
    }

    int currentSliderRow = 0;

    int currentSliderColumn = 1;

    int rowWidth = (reverbArea.getWidth() / 6.0);

  

    int columnHeight = (reverbArea.getHeight() / 2.0);

    int middleCompensation =  rowWidth / 2;

    int columnCompensation = columnHeight / 2;

    int TextCompensation = sliderSize / 2;

    //label setting

    roomSizeLabel.setColour(juce::Label::textColourId, fontColor);
    dampingLabel.setColour(juce::Label::textColourId, fontColor);
    dryLabel.setColour(juce::Label::textColourId, fontColor);
    wetLabel.setColour(juce::Label::textColourId, fontColor);
    widthLabel.setColour(juce::Label::textColourId, fontColor);

  //  roomSizeLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 10.0)));

    roomSizeSlider.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation, sliderSize, sliderSize);

    roomSizeLabel.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation  - (reverbArea.getHeight() * (1.0 / 10.0)), sliderSize, reverbArea.getHeight() * (1.0 / 10.0));

    ++currentSliderRow;
    currentSliderColumn = 2;

  //  dampingLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 8.0)));

    dampingSlider.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation, sliderSize, sliderSize);
    dampingLabel.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation  - (reverbArea.getHeight() * (1.0 / 10.0)), sliderSize, reverbArea.getHeight() * (1.0 / 10.0));


    ++currentSliderRow;
    currentSliderColumn = 1;


  //  dryLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 6.0)));

    drySlider.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation, sliderSize, sliderSize);
    dryLabel.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation  - (reverbArea.getHeight() * (1.0 / 10.0)), sliderSize, reverbArea.getHeight() * (1.0 / 10.0));

    ++currentSliderRow;
    currentSliderColumn = 2;


  //  wetLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 4.0)));

    wetSlider.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn)+ columnCompensation, sliderSize, sliderSize);
    wetLabel.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation  - (reverbArea.getHeight() * (1.0 / 10.0)), sliderSize, reverbArea.getHeight() * (1.0 / 10.0));

  //  widthLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 2.0)));
    currentSliderColumn = 1;

    ++currentSliderRow;



    widthSlider.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation, sliderSize, sliderSize);
    widthLabel.setBounds((rowWidth * currentSliderRow) + middleCompensation, (columnHeight * currentSliderColumn) + columnCompensation  - (reverbArea.getHeight() * (1.0 / 10.0)), sliderSize, reverbArea.getHeight() * (1.0 / 10.0));


    auto   convolutionButtonBounds = convolutionArea;
    auto convolutionToggleBounds = convolutionButtonBounds.removeFromLeft(convolutionButtonBounds.getWidth() * 0.5);
    convolutionLabel.setBounds(convolutionToggleBounds.removeFromRight(convolutionToggleBounds.getWidth() * 0.8));

    convolutionToggle.setBounds(convolutionArea.removeFromLeft(convolutionArea.getWidth()* ToggleSize));

    convolutionLabel.setColour(juce::Label::textColourId, fontColor);


    loadBtn.setBounds(convolutionButtonBounds);


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

