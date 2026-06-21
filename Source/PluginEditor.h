/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "smallKnob1.h"
#include "smallKnob2.h"
#include "overlappingBigKnob.h"
#include "skinButton.h"
//==============================================================================
/**
*/
class Squwbs4AudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                    public juce::LookAndFeel_V4,
                                    public juce::Button::Listener,
                                    public juce::TextEditor::Listener,
                                    public juce::Timer
{
public:
    Squwbs4AudioProcessorEditor (Squwbs4AudioProcessor&);
    ~Squwbs4AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button) override;
    void timerCallback() override;
    void textEditorReturnKeyPressed (juce::TextEditor& textEditor) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Squwbs4AudioProcessor& audioProcessor;
    
    // License UI components
    juce::Label licenseLabel;
    juce::TextEditor licenseKeyInput;
    juce::TextButton sendButton { "Send" };
    juce::Label errorLabel;
    bool isLicensed = false;
    bool isValid = false;
    bool isDarkMode = true;
    int failedAttempts = 0;
    
    // Main UI components
    juce::Slider gainSlider;
    //juce::Slider LPSlider;
    juce::Slider doublerSlider;
    juce::Slider volSlider;
    OverlappingBigKnob overlappingBigKnob;
    SmallKnob1 myCustomLookAndFeel1;
    SmallKnob2 myCustomLookAndFeel2;
    SkinButton skinButton;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttachment;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LPAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    
    void showLicenseScreen();
    void showMainUI();
    void handleLicenseValidation (const juce::String& licenseKey);
    void updateSkinMode();
    void showError (const juce::String& message);

    float errorAlpha = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Squwbs4AudioProcessorEditor)
};
