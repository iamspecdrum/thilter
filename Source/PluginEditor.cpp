/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Squwbs4AudioProcessorEditor::Squwbs4AudioProcessorEditor (Squwbs4AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    overlappingBigKnob (BinaryData::bigknobimagestrip_png, BinaryData::bigknobimagestrip_pngSize, 128),
    overlappingBigKnobLight (BinaryData::bigknobimagestrip_light_png, BinaryData::bigknobimagestrip_light_pngSize, 128),
    skinButton ("Button",BinaryData::buttonimagestrip_png, BinaryData::buttonimagestrip_pngSize),
    skinButtonLight ("Button",BinaryData::buttonimagestrip_light_png, BinaryData::buttonimagestrip_light_pngSize)
{
    audioProcessor.parameters.addParameterListener("SKIN_ID", this);

    if (auto* skinParameter = audioProcessor.parameters.getRawParameterValue("SKIN_ID"))
        isDarkMode = skinParameter->load() > 0.5f;

    // Initialize license screen
    showLicenseScreen();
    //skinButton.addListener(this);
    //skinButton.setToggleState(!isDarkMode, juce::dontSendNotification);
    setSize (341, 420);
    
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
    licenseLabel.setColour (juce::Label::backgroundColourId, juce::Colour (29, 29, 29));
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
    licenseKeyInput.setColour (juce::TextEditor::backgroundColourId, juce::Colour (29, 29, 29));
    licenseKeyInput.setColour (juce::TextEditor::textColourId, juce::Colours::white);
    licenseKeyInput.setColour (juce::TextEditor::highlightColourId, juce::Colour (150, 150, 255));
    licenseKeyInput.setFont (juce::Font (16.0f));
    licenseKeyInput.addListener (this);
    addAndMakeVisible (licenseKeyInput);

    errorLabel.setText ("", juce::dontSendNotification);
    errorLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    errorLabel.setColour (juce::Label::textWhenEditingColourId, juce::Colour (230, 100, 100));
    errorLabel.setColour (juce::Label::backgroundColourId, juce::Colour (29, 29, 29));
    errorLabel.setOpaque (true);
    errorLabel.setFont (juce::Font (15.0f, juce::Font::bold));
    errorLabel.setJustificationType (juce::Justification::centred);
    errorLabel.setVisible (false);
    addAndMakeVisible (errorLabel);
    
    // Setup send button
    sendButton.addListener (this);
    sendButton.setColour (juce::TextButton::buttonColourId, juce::Colour (29, 29, 29));
    sendButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (175, 175, 175));
    sendButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    sendButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible (sendButton);

    isLicensed = false;
    isValid = false;   

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
    setSize (341, 420);
    addAndMakeVisible (overlappingBigKnob);
    addAndMakeVisible (overlappingBigKnobLight);

    doublerSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    doublerSlider.setLookAndFeel(&myCustomLookAndFeel1);
    doublerSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    doublerSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (29, 29, 29));
    doublerSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    doublerSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80, 80, 80));
    doublerSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(doublerSlider);
    
    doublerSliderLight.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    doublerSliderLight.setLookAndFeel(&myCustomLookAndFeel1Light);
    doublerSliderLight.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    doublerSliderLight.setColour (juce::Slider::backgroundColourId, juce::Colour (29, 29, 29));
    doublerSliderLight.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    doublerSliderLight.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80, 80, 80));
    doublerSliderLight.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(doublerSliderLight);
    
    
    volSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    volSlider.setLookAndFeel(&myCustomLookAndFeel2);
    volSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (29, 29, 29));
    volSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    volSlider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80, 80, 80));
    volSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(volSlider);

    volSliderLight.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    volSliderLight.setLookAndFeel(&myCustomLookAndFeel2Light);
    volSliderLight.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volSliderLight.setColour (juce::Slider::backgroundColourId, juce::Colour (29, 29, 29));
    volSliderLight.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    volSliderLight.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(80, 80, 80));
    volSliderLight.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(volSliderLight);

    skinButton.addListener (this);
    skinButtonLight.addListener (this);
    addAndMakeVisible(skinButton);
    addAndMakeVisible(skinButtonLight);


    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "GAIN_ID", overlappingBigKnob);
    gainAttachmentLight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "GAIN_ID", overlappingBigKnobLight);
    volAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "VOL_ID", volSlider);
    volAttachmentLight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "VOL_ID", volSliderLight);

    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "WIDTH_ID", doublerSlider);
    widthAttachmentLight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "WIDTH_ID", doublerSliderLight);
    updateSkinMode();

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
    overlappingBigKnob.setLookAndFeel(nullptr);
    overlappingBigKnobLight.setLookAndFeel(nullptr);
    //LPSlider.setLookAndFeel(nullptr);
    volSlider.setLookAndFeel(nullptr);
    doublerSlider.setLookAndFeel(nullptr);
    sendButton.removeListener (this);
    licenseKeyInput.removeListener (this);
    skinButton.removeListener (this);
    skinButtonLight.removeListener (this);
    audioProcessor.parameters.removeParameterListener("SKIN_ID", this);
    stopTimer();
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
    else if (button == &skinButton || button == &skinButtonLight)
    {
        isDarkMode = !isDarkMode;

        if (auto* parameter = audioProcessor.parameters.getParameter("SKIN_ID"))
            parameter->setValueNotifyingHost(isDarkMode ? 1.0f : 0.0f);

        updateSkinMode();
    }
}

void Squwbs4AudioProcessorEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "SKIN_ID")
    {
        const bool newDarkMode = newValue > 0.5f;
        if (isDarkMode != newDarkMode)
        {
            isDarkMode = newDarkMode;
            updateSkinMode();
        }
    }
}

void Squwbs4AudioProcessorEditor::updateSkinMode()
{
    if (skinButton.isVisible())
        skinButton.setToggleState(isDarkMode, juce::dontSendNotification);
    if (skinButtonLight.isVisible())
        skinButtonLight.setToggleState(!isDarkMode, juce::dontSendNotification);

    // Show/hide components according to the current skin. Parameter attachments keep both
    // dark and light controls synchronized with the processor values, so no referTo needed.
    repaint();
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
    g.fillAll(juce::Colour (29, 29, 29));
    if(!isDarkMode){
        overlappingBigKnob.setBounds(0, 0, 341, 420);
        overlappingBigKnob.setVisible(true);
        overlappingBigKnobLight.setVisible(false);
        doublerSlider.setBounds(38,249,90,90);
        doublerSlider.setVisible(true);
        doublerSliderLight.setVisible(false);
        //doublerSlider.setLookAndFeel(&myCustomLookAndFeel1);
        volSlider.setBounds(212,249,90,90);
        volSlider.setVisible(true);
        volSliderLight.setVisible(false);
        //volSlider.setLookAndFeel(&myCustomLookAndFeel2);
        skinButton.setBounds(150,361,40,40);
        skinButton.setVisible(true);
        skinButtonLight.setVisible(false);
        
        
        
    }
    else{
        overlappingBigKnobLight.setBounds(0, 0, 341, 420);
        overlappingBigKnobLight.setVisible(true);
        overlappingBigKnob.setVisible(false);
        doublerSliderLight.setBounds(38,249,90,90);
        doublerSliderLight.setVisible(true);
        doublerSlider.setVisible(false);
        //doublerSlider.setLookAndFeel(&myCustomLookAndFeel1Light);
        volSliderLight.setBounds(212,249,90,90);
        volSliderLight.setVisible(true);
        volSlider.setVisible(false);
        //volSlider.setLookAndFeel(&myCustomLookAndFeel2Light);
        skinButtonLight.setBounds(150,361,40,40);
        skinButtonLight.setVisible(true);
        skinButton.setVisible(false);
     // Skip layout adjustments for light mode
    }
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
        //gainSlider.setBounds (0, 0, 341, 420);
        
        skinButton.setBounds(150,361,40,40);
        
        
        
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
