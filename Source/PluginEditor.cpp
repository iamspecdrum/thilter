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
    setSize (250, 250);
}
juce::File getLicenseFile() {
    DBG("getLicenseFile ran");
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("squwbs").getChildFile("Thilter").getChildFile("license.settings");
        
}
void saveLicenseFile(const juce::String& key) {
    auto file = getLicenseFile();
    file.getParentDirectory().createDirectory();
    file.replaceWithText(key);
    DBG("saveLicenseFile ran");
    DBG(file.getFullPathName());// Better: Encrypt/Sign this data
}
/*
void saveLicenseLocally (const juce::String& licenseKey, const juce::String& instanceId)
{

    auto appDataDir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory);

    // 2. Build the full path: AppData / CompanyName / AppName / license.settings
    // Use getChildFile to ensure platform-specific separators are handled correctly
    auto licenseFile = appDataDir.getChildFile ("squwbs")
                                 .getChildFile ("Thilter")
                                 .getChildFile ("license.settings");

    // 3. Ensure the parent directories exist before saving
    if (! licenseFile.getParentDirectory().exists())
        licenseFile.getParentDirectory().createDirectory();

    // 4. Setup Options (Options are still needed for the storage format)
    juce::PropertiesFile::Options options;
    options.storageFormat = juce::PropertiesFile::storeAsXML;

    // 5. Create PropertiesFile by passing the exact File object
    juce::PropertiesFile settings (licenseFile, options);

    // 6. Save the data
    settings.setValue ("license_key", licenseKey);
    settings.setValue ("instance_id", instanceId);
    settings.saveIfNeeded();
    
    DBG ("License saved to: " << licenseFile.getFullPathName());

}
*/
struct LicenseData { juce::String key; juce::String instance; };

LicenseData loadLicenseLocally()
{
// 1. Reconstruct the exact same path used in saveLicenseLocally
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto licenseFile = appDataDir.getChildFile("squwbs")
                                 .getChildFile("Thilter")
                                 .getChildFile("license.settings");

    // 2. Check if the file actually exists
    if (!licenseFile.existsAsFile())
    {
        DBG("No license file found at: " << licenseFile.getFullPathName());
        return { "", "" }; // Return empty strings if file doesn't exist
    }

    // 3. Setup the same options (matching the storage format)
    juce::PropertiesFile::Options options;
    options.storageFormat = juce::PropertiesFile::storeAsXML;

    // 4. Load the file
    juce::PropertiesFile settings (licenseFile, options);

    // 5. Retrieve values (providing an empty string as the default if key isn't found)
    LicenseData data;
    data.key        = settings.getValue("license_key", "");
    data.instance = settings.getValue("instance_id", "");

    return data;
}
void Squwbs4AudioProcessorEditor::showLicenseScreen()
{
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

    isLicensed = false;
    isValid = false;   
    /*
    auto file = getLicenseFile();
    if (file.existsAsFile()) {
        auto savedKey = file.loadFileAsString();
    if (audioProcessor.validateLicense (savedKey)) // Implement your key verification logic
        isLicensed = true;
        DBG("License key from file is valid. Skipping user input validation.");
    }
    failedAttempts = 0;  // Reset failed attempts counter
    
    if (isLicensed)
    {
        showMainUI();
        DBG("License validation successful. Showing main UI.");
    }
    */
    auto file = getLicenseFile();
    if(file.existsAsFile())
    {
        auto data = loadLicenseLocally();
        if (audioProcessor.validateLicense (data.key,data.instance)) // Implement your key verification logic
        {
            isLicensed = true;
            DBG("License key from file is valid. Skipping user input validation.");
            showMainUI();
            DBG("License validation successful. Showing main UI.");
        }
    }

}

void Squwbs4AudioProcessorEditor::showMainUI()
{
    // Clear any existing components
    removeAllChildren();
    setSize (250, 250);
    // Configure and add the gain slider
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    //gainSlider.setLookAndFeel(&myCustomLookAndFeel);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (56, 56, 56));
    gainSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    gainSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80,80,80));
    gainSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible (gainSlider);
    //LPSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    //gainSlider.setLookAndFeel(&myCustomLookAndFeel);
    //LPSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    //LPSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (56, 56, 56));
    //LPSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (80, 80, 80));
    //LPSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    //LPSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    //addAndMakeVisible(LPSlider);
    doublerSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    //gainSlider.setLookAndFeel(&myCustomLookAndFeel);
    doublerSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    doublerSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (56, 56, 56));
    doublerSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    doublerSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80, 80, 80));
    doublerSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(doublerSlider);
    volSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    //gainSlider.setLookAndFeel(&myCustomLookAndFeel);
    volSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (56, 56, 56));
    volSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    volSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80, 80, 80));
    volSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(volSlider);

    // Create the slider attachment
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "GAIN_ID", gainSlider);
    volAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "VOL_ID", volSlider);
    //LPAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.parameters, "LP_ID", LPSlider);
    doublerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
    audioProcessor.parameters, "DOUBLER_ID", doublerSlider);
    
    
}

void Squwbs4AudioProcessorEditor::showError (const juce::String& message)
{
    errorAlpha = 1.0f;
    errorLabel.setText (message, juce::dontSendNotification);
    errorLabel.setVisible (true);
    errorLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (errorAlpha));
    startTimerHz (30);
}
/*
void Squwbs4AudioProcessorEditor::handleLicenseValidation (const juce::String& licenseKey)
{
    auto file = getLicenseFile();
    if (file.existsAsFile()) {
        auto savedKey = file.loadFileAsString();
    if (audioProcessor.validateLicense (savedKey)) // Implement your key verification logic
        isLicensed = true;
        DBG("License key from file is valid. Skipping user input validation.");
    }
    
    // Call the processor's validation function
    if (!isLicensed) {
        //juce::DBG("License key from file is invalid. Proceeding with user input validation.");
         isValid = audioProcessor.validateLicense (licenseKey);
    }
    
    
    if (isLicensed||isValid)
    {
        //errorLabel.setVisible (false);
        showMainUI();
        DBG("License validation successful. Showing main UI.");
        stopTimer();
        saveLicenseFile(licenseKey); // Save the valid key for future sessions
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
*/

void Squwbs4AudioProcessorEditor::handleLicenseValidation (const juce::String& licenseKey)
{
    auto file = getLicenseFile();
    DBG("file exitsts "+file.existsAsFile());
    if (file.existsAsFile()) {
        auto data = loadLicenseLocally();
        if (audioProcessor.validateLicense (data.key,data.instance)){
            isLicensed = true;
            DBG("License key from file is valid. Skipping user input validation.");
        }   
    }
    
    // Call the processor's validation function
    if (!isLicensed) {
        //juce::DBG("License key from file is invalid. Proceeding with user input validation.");
         isValid = audioProcessor.activateLicense (licenseKey);
    }
    
    
    if (isLicensed||isValid)
    {
        //errorLabel.setVisible (false);
        showMainUI();
        DBG("License validation successful. Showing main UI.");
        stopTimer();
        //saveLicenseFile(licenseKey); // Save the valid key for future sessions
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
    //LPSlider.setLookAndFeel(nullptr);
    volSlider.setLookAndFeel(nullptr);
    doublerSlider.setLookAndFeel(nullptr);
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
    
    if (isLicensed||isValid)
    {
        // Main UI layout
        gainSlider.setBounds (25, 12, 200, 200);
        volSlider.setBounds(194,200,50,50);
        doublerSlider.setBounds(6,200,50,50);
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
