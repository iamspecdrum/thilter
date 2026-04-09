/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "customKnob.h"
//==============================================================================
/**
*/
class Squwbs4AudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::LookAndFeel_V4
{
public:
    Squwbs4AudioProcessorEditor (Squwbs4AudioProcessor&);
    ~Squwbs4AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Squwbs4AudioProcessor& audioProcessor;
    
    juce::Slider gainSlider;
    CustomRotaryKnobLookAndFeel myCustomLookAndFeel;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Squwbs4AudioProcessorEditor)
};
