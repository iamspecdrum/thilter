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
    // Initialize license screen
    showLicenseScreen();
    setSize (400, 300);
}

void Squwbs4AudioProcessorEditor::showLicenseScreen()
{
    isLicensed = false;
    failedAttempts = 0;  // Reset failed attempts counter
    
    // Clear any existing components
    removeAllChildren();
    
    // Setup license label
    licenseLabel.setText ("Enter License Key", juce::dontSendNotification);
    licenseLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    licenseLabel.setColour (juce::Label::backgroundColourId, juce::Colour (56, 56, 56));
    licenseLabel.setOpaque (true);
    addAndMakeVisible (licenseLabel);
    
    // Setup license key input
    licenseKeyInput.setMultiLine (false);
    licenseKeyInput.setReturnKeyStartsNewLine (false);
    licenseKeyInput.setReadOnly (false);
    licenseKeyInput.setScrollbarsShown (false);
    licenseKeyInput.setCaretVisible (true);
    licenseKeyInput.setPopupMenuEnabled (true);
    licenseKeyInput.setText ("");
    licenseKeyInput.setColour (juce::TextEditor::backgroundColourId, juce::Colour (80, 80, 80));
    licenseKeyInput.setColour (juce::TextEditor::textColourId, juce::Colours::white);
    licenseKeyInput.setColour (juce::TextEditor::highlightColourId, juce::Colour (150, 150, 255));
    licenseKeyInput.setFont (juce::Font (16.0f));
    licenseKeyInput.addListener (this);
    addAndMakeVisible (licenseKeyInput);

    errorLabel.setText ("", juce::dontSendNotification);
    errorLabel.setColour (juce::Label::textColourId, juce::Colour (230, 100, 100));
    errorLabel.setColour (juce::Label::backgroundColourId, juce::Colour (56, 56, 56));
    errorLabel.setOpaque (true);
    errorLabel.setFont (juce::Font (15.0f, juce::Font::bold));
    errorLabel.setJustificationType (juce::Justification::centred);
    errorLabel.setVisible (false);
    addAndMakeVisible (errorLabel);
    
    // Setup send button
    sendButton.addListener (this);
    sendButton.setColour (juce::TextButton::buttonColourId, juce::Colour (70, 70, 70));
    sendButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (90, 90, 90));
    sendButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    sendButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible (sendButton);
}

void Squwbs4AudioProcessorEditor::showMainUI()
{
    isLicensed = true;
    
    // Clear any existing components
    removeAllChildren();
    
    // Configure and add the gain slider
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    gainSlider.setLookAndFeel(&myCustomLookAndFeel);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (56, 56, 56));
    gainSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (80, 80, 80));
    gainSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    gainSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible (gainSlider);

    // Create the slider attachment
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "GAIN_ID", gainSlider);
    
    setSize (250, 250);
}

void Squwbs4AudioProcessorEditor::showError (const juce::String& message)
{
    errorAlpha = 1.0f;
    errorLabel.setText (message, juce::dontSendNotification);
    errorLabel.setVisible (true);
    errorLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (errorAlpha));
    startTimerHz (30);
}

void Squwbs4AudioProcessorEditor::handleLicenseValidation (const juce::String& licenseKey)
{
    // Call the processor's validation function
    bool isValid = audioProcessor.validateLicense (licenseKey);
    
    if (isValid)
    {
        errorLabel.setVisible (false);
        stopTimer();
        showMainUI();
    }
    else
    {
        failedAttempts++;
        
        if (failedAttempts >= 5)
        {
            // Lock the input and show reload message
            licenseKeyInput.setReadOnly (true);
            licenseKeyInput.setText ("");
            sendButton.setEnabled (false);
            errorLabel.setText ("Too many failed attempts. Please reload the plugin to try again.", juce::dontSendNotification);
            errorLabel.setColour (juce::Label::textColourId, juce::Colour (230, 100, 100));
            errorLabel.setVisible (true);
            stopTimer();  // Make sure any fading timer is stopped
        }
        else
        {
            showError ("Invalid license key");
            licenseKeyInput.setText ("");
        }
    }
}


Squwbs4AudioProcessorEditor::~Squwbs4AudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);
    sendButton.removeListener (this);
    licenseKeyInput.removeListener (this);
}

void Squwbs4AudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &sendButton && sendButton.isEnabled())
    {
        juce::String licenseKey = licenseKeyInput.getText();
        if (licenseKey.isEmpty())
        {
            showError ("Empty license key");
        }
        else
        {
            handleLicenseValidation (licenseKey);
        }
    }
}

void Squwbs4AudioProcessorEditor::textEditorReturnKeyPressed (juce::TextEditor& textEditor)
{
    if (&textEditor == &licenseKeyInput && sendButton.isEnabled())
    {
        juce::String licenseKey = licenseKeyInput.getText();
        if (licenseKey.isEmpty())
        {
            showError ("Empty license key");
        }
        else
        {
            handleLicenseValidation (licenseKey);
        }
    }
}

//==============================================================================
void Squwbs4AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour (56, 56, 56));
}

void Squwbs4AudioProcessorEditor::timerCallback()
{
    errorAlpha -= 0.05f;
    if (errorAlpha <= 0.0f)
    {
        errorAlpha = 0.0f;
        errorLabel.setVisible (false);
        stopTimer();
    }
    else
    {
        errorLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (errorAlpha));
    }
}

void Squwbs4AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    if (isLicensed)
    {
        // Main UI layout
        gainSlider.setBounds (0, 0, 250, 250);
    }
    else
    {
        // License UI layout
        int margin = 20;
        int labelHeight = 30;
        int inputHeight = 32;
        int errorHeight = 24;
        int buttonHeight = 40;
        int spacing = 10;
        
        licenseLabel.setBounds (margin, margin, getWidth() - 2*margin, labelHeight);
        licenseKeyInput.setBounds (margin, margin + labelHeight + spacing, getWidth() - 2*margin, inputHeight);
        errorLabel.setBounds (margin, margin + labelHeight + spacing + inputHeight + spacing, getWidth() - 2*margin, errorHeight);
        sendButton.setBounds (margin, margin + labelHeight + spacing + inputHeight + spacing + errorHeight + spacing, getWidth() - 2*margin, buttonHeight);
        
        // Reduce bottom margin by setting window height to fit content more tightly
        int totalHeight = margin + labelHeight + spacing + inputHeight + spacing + errorHeight + spacing + buttonHeight + margin;
        setSize (400, totalHeight);
    }
}
