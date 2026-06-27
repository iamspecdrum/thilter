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
#include "smallKnob1Light.h"
#include "smallKnob2Light.h"
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
    juce::Slider gainSliderLight;
    //juce::Slider LPSlider;
    juce::Slider doublerSlider;
    juce::Slider doublerSliderLight;
    juce::Slider volSlider;
    juce::Slider volSliderLight;
    OverlappingBigKnob overlappingBigKnob;
    OverlappingBigKnob overlappingBigKnobLight;
    SmallKnob1 myCustomLookAndFeel1;
    SmallKnob2 myCustomLookAndFeel2;
    SmallKnob1Light myCustomLookAndFeel1Light;
    SmallKnob2Light myCustomLookAndFeel2Light;
    SkinButton skinButton;
    SkinButton skinButtonLight;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttachment;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LPAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    // Attachments for light-mode controls (keep separate to avoid overwriting)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachmentLight;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttachmentLight;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachmentLight;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachmentLight;

    void showLicenseScreen();
    void showMainUI();
    void handleLicenseValidation (const juce::String& licenseKey);
    void updateSkinMode();
    void showError (const juce::String& message);

    float errorAlpha = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Squwbs4AudioProcessorEditor)
};
