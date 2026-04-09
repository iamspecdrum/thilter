/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Squwbs4AudioProcessorEditor::Squwbs4AudioProcessorEditor (Squwbs4AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // Configure and add the gain slider
        //gainSlider.setSliderStyle (juce::Slider::LinearVertical);
        //gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 50, 20);
        gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
        gainSlider.setLookAndFeel(&myCustomLookAndFeel);
        gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    
        //gainSlider.setRange(0.0f,100.0f,1.0f);
        //gainSlider.setValue(50.0f);
        addAndMakeVisible (gainSlider);

        // Configure and add the gain label
        //gainLabel.setText ("DRY/WET", juce::dontSendNotification);
        //gainLabel.attachToComponent (&gainSlider, false);
        //addAndMakeVisible (gainLabel);
        //gainLabel.setText(std::to_string(audioProcessor.mixFloat), juce::dontSendNotification);
        // Create the slider attachment
        gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "GAIN_ID", gainSlider);

        setSize (220, 220);
}

Squwbs4AudioProcessorEditor::~Squwbs4AudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void Squwbs4AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void Squwbs4AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    gainSlider.setBounds (getWidth()/2-50, getHeight()/2-50, 100, 100);
}
