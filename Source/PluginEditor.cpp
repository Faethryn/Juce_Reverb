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

//responsecurve

ResponseCurveComponent::ResponseCurveComponent(JuceReverbAudioProcessor& p) :
    audioProcessor(p),
    leftPathProducer(audioProcessor.leftChannelFifo),
    rightPathProducer(audioProcessor.rightChannelFifo),
    leftInputPathProducer(audioProcessor.leftInputChannelFifo),
    rightInputPathProducer(audioProcessor.rightInputChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    

    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::updateResponseCurve()
{
    using namespace juce;
    auto responseArea = getAnalysisArea();

    auto w = responseArea.getWidth();

   

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; ++i)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

       

        mags[i] = Decibels::gainToDecibels(mag);
    }

    responseCurve.clear();

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); ++i)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }
}

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    drawBackgroundGrid(g);

    auto responseArea = getAnalysisArea();

    if (shouldShowFFTAnalysis)
    {
        auto leftChannelFFTPath = leftPathProducer.getPath();
        leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));

        g.setColour(Colour(97u, 18u, 167u)); //purple-
        g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

        auto rightChannelFFTPath = rightPathProducer.getPath();
        rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));

        g.setColour(Colour(215u, 201u, 134u));
        g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));

        auto leftInputChannelFFTPath = leftInputPathProducer.getPath();
        leftInputChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));

        g.setColour(juce::Colours::red); 
        g.strokePath(leftInputChannelFFTPath, PathStrokeType(1.f));

        auto rightInputChannelFFTPath = rightInputPathProducer.getPath();
        rightInputChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));

        g.setColour(juce::Colours::green);
        g.strokePath(rightInputChannelFFTPath, PathStrokeType(1.f));
    }

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));

    Path border;

    border.setUsingNonZeroWinding(false);

    border.addRoundedRectangle(getRenderArea(), 4);
    border.addRectangle(getLocalBounds());

    g.setColour(Colours::black);

    g.fillPath(border);

    drawTextLabels(g);

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
}

std::vector<float> ResponseCurveComponent::getFrequencies()
{
    return std::vector<float>
    {
        20, /*30, 40,*/ 50, 100,
            200, /*300, 400,*/ 500, 1000,
            2000, /*3000, 4000,*/ 5000, 10000,
            20000
    };
}

std::vector<float> ResponseCurveComponent::getGains()
{
    return std::vector<float>
    {
        -24, -12, 0, 12, 24
    };
}

std::vector<float> ResponseCurveComponent::getXs(const std::vector<float>& freqs, float left, float width)
{
    std::vector<float> xs;
    for (auto f : freqs)
    {
        auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
        xs.push_back(left + width * normX);
    }

    return xs;
}

void ResponseCurveComponent::drawBackgroundGrid(juce::Graphics& g)
{
    using namespace juce;
    auto freqs = getFrequencies();

    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto xs = getXs(freqs, left, width);

    g.setColour(Colours::dimgrey);
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }

    auto gain = getGains();

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));

        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
}

void ResponseCurveComponent::drawTextLabels(juce::Graphics& g)
{
    using namespace juce;
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);

    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();

    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto freqs = getFrequencies();
    auto xs = getXs(freqs, left, width);

    for (int i = 0; i < freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }

        str << f;
        if (addK)
            str << "k";
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;

        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(1);

        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }

    auto gain = getGains();

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));

        String str;
        if (gDb > 0)
            str << "+";
        str << gDb;

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);

        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey);

        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);

        str.clear();
        str << (gDb - 24.f);

        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

void ResponseCurveComponent::resized()
{
    using namespace juce;

    responseCurve.preallocateSpace(getWidth() * 3);
    updateResponseCurve();
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))
        {
            auto size = tempIncomingBuffer.getNumSamples();

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                monoBuffer.getReadPointer(0, size),
                monoBuffer.getNumSamples() - size);

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                tempIncomingBuffer.getReadPointer(0, 0),
                size);

            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
        }
    }

    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / double(fftSize);

    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }

    while (pathProducer.getNumPathsAvailable() > 0)
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}

void ResponseCurveComponent::timerCallback()
{
    if (shouldShowFFTAnalysis)
    {
        auto fftBounds = getAnalysisArea().toFloat();
        auto sampleRate = audioProcessor.getSampleRate();

        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
        leftInputPathProducer.process(fftBounds, sampleRate);
        rightInputPathProducer.process(fftBounds, sampleRate);
    }

    if (parametersChanged.compareAndSetBool(false, true))
    {
       
        updateResponseCurve();
    }

    repaint();
}


juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}


juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
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
    responseCurveComponent(audioProcessor),
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

    responseCurveComponent.setBounds(responseArea);
   
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

    widthSlider.setBounds(topRowArea);
    widthLabel.setBounds(widthSlider.getBounds().getX(), (labelHeightFirstRow), widthSlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);

 


  //  wetLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 4.0)));

    wetSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));

    auto labelHeightSecondRow = wetSlider.getBounds().getY() /*- (getLocalBounds().getHeight() * labelHeight)*/ /*- (wetSlider.getBounds().getHeight() / 2)*/;


    wetLabel.setBounds(wetSlider.getBounds().getX(), (labelHeightSecondRow), wetSlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);

  //  widthLabel.setBounds(reverbArea.removeFromTop(reverbArea.getHeight() * (1.0 / 2.0)));
  



    drySlider.setBounds(bounds);
    dryLabel.setBounds(drySlider.getBounds().getX(), (labelHeightSecondRow), drySlider.getBounds().getWidth(), getLocalBounds().getHeight() * labelHeight);


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
         & responseCurveComponent,
        &convolutionLabel,
        &reverbLabel,
        &roomSizeLabel,
        &dampingLabel, 
        &wetLabel, 
        &dryLabel, 
        &widthLabel

    };
}

