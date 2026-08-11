/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Squwbs4AudioProcessor::Squwbs4AudioProcessor()
:AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
parameters (*this, &undoManager, "Parameters", createParameterLayout())
{
    parameters.addParameterListener("GAIN_ID",this);
    gainParameter = parameters.getRawParameterValue("GAIN_ID");
    parameters.addParameterListener("VOL_ID",this);
    volParameter = parameters.getRawParameterValue("VOL_ID");
    //parameters.addParameterListener("LP_ID",this);
    //LPParameter = parameters.getRawParameterValue("LP_ID");
    parameters.addParameterListener("WIDTH_ID",this);
    widthParameter = parameters.getRawParameterValue("WIDTH_ID");
    parameters.addParameterListener("SKIN_ID",this);
    buttonParameter = parameters.getRawParameterValue("SKIN_ID");
    parameters.addParameterListener("CHARACTER_ID",this);
    secretCharacterParameter = parameters.getRawParameterValue("CHARACTER_ID");

}

Squwbs4AudioProcessor::~Squwbs4AudioProcessor()
{
    parameters.removeParameterListener("GAIN_ID",this);
    parameters.removeParameterListener("VOL_ID",this);
    //parameters.removeParameterListener("LP_ID",this);
    parameters.removeParameterListener("WIDTH_ID",this);
    parameters.removeParameterListener("SKIN_ID",this);
    parameters.removeParameterListener("CHARACTER_ID",this);
}
void Squwbs4AudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if(parameterID == "GAIN_ID")
    {
        juce::ignoreUnused(newValue);
    }
    if(parameterID == "VOL_ID")
    {
        juce::ignoreUnused(newValue);
    }
    /*
    if(parameterID == "LP_ID")
    {
        juce::ignoreUnused(newValue);
    }
    */
    if(parameterID == "WIDTH_ID")
    {
        juce::ignoreUnused(newValue);
    }

    if(parameterID == "SKIN_ID")
    {
        juce::ignoreUnused(newValue);
    }

    if(parameterID == "CHARACTER_ID")
    {
        juce::ignoreUnused(newValue);
    }
}

void Squwbs4AudioProcessor::saveLicenseLocally (const juce::String& licenseKey, const juce::String& instanceId)
{
    auto appDataDir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory);
    auto licenseFile = appDataDir.getChildFile ("squwbs")
                                 .getChildFile ("Thilter")
                                 .getChildFile ("license.settings");
    if (! licenseFile.getParentDirectory().exists())
        licenseFile.getParentDirectory().createDirectory();
    juce::PropertiesFile::Options options;
    options.storageFormat = juce::PropertiesFile::storeAsXML;
    juce::PropertiesFile settings (licenseFile, options);
    settings.setValue ("license_key", licenseKey);
    settings.setValue ("instance_id", instanceId);
    settings.saveIfNeeded();
    DBG ("License saved to: " << licenseFile.getFullPathName());
}


bool Squwbs4AudioProcessor::activateLicense (const juce::String& licenseKey)
{
    // TODO: Implement actual API communication with your license server
    // This is a placeholder implementation
    
    // Placeholder URL for your license validation API
    // Example: "https://your-api.com/validate-license"
    //juce::String validationUrl = "https://api.example.com/validate-license";
    juce::URL url("https://api.lemonsqueezy.com/v1/licenses/activate");
    //juce::DBG("Validating license key: " + licenseKey);
    //juce::DBG("API URL: " + validationUrl);
    
    // Placeholder: For demonstration, accept any non-empty key that starts with "VALID"
    // In production, replace this with actual HTTP request to your API
    /*
    if (licenseKey.startsWith("VALID"))
    {
        //juce::DBG("License validation successful");
        return true;
    }
    else
    {
        //juce::DBG("License validation failed");
        return false;
    }
    */

    url = url
         .withParameter ("license_key", licenseKey)
         .withParameter ("instance_name", juce::SystemStats::getComputerName());
    juce::String pairHeaders = "Accept: application/json\n"
                                   "Content-Type: application/x-www-form-urlencoded";
    auto options = juce::URL::InputStreamOptions (juce::URL::ParameterHandling::inPostData)
                        .withExtraHeaders (pairHeaders)
                        .withConnectionTimeoutMs (10000);

    // 4. Send the request (usually on a background thread)
    if (auto stream = url.createInputStream (options))
    {
        auto response = stream->readEntireStreamAsString();
        auto json = juce::JSON::parse (response);
        if ((bool)json["activated"])
        {
            // Store the instance ID for future validation
            auto instanceId = json["instance"]["id"].toString();
            
            saveLicenseLocally(licenseKey, instanceId);
            DBG("Activation Successful! Instance ID: " << instanceId);
            return true;
        }
        else
        {
            DBG(json.toString());
            DBG("Activation Failed: " << json["error"].toString());
            return false;
        }
    }
}
//==============================================================================
bool Squwbs4AudioProcessor::validateLicense (const juce::String& licenseKey, const juce::String& instanceId)
{
    juce::URL url("https://api.lemonsqueezy.com/v1/licenses/validate");
    url = url.withParameter("license_key", licenseKey)
             .withParameter("instance_id", instanceId);

    auto options = juce::URL::InputStreamOptions (juce::URL::ParameterHandling::inPostData)
                        .withExtraHeaders ("Accept: application/json");

    if (auto stream = url.createInputStream (options))
    {
        auto json = juce::JSON::parse (stream->readEntireStreamAsString());
        
        bool isValid = (bool)json["valid"];
        bool sameName = json["instance"]["name"].toString()==juce::SystemStats::getComputerName();
        if (isValid&&sameName)
        {
             DBG("License is still active.");
             return true;
        }
        else
        {
            if(isValid){
                DBG("computer name not matching");
            }
            else{
                DBG("License invalid or expired.");
            }
            auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
            auto licenseFile = appDataDir.getChildFile("squwbs")
                .getChildFile("Thilter")
                .getChildFile("license.settings");
            licenseFile.deleteFile();
            return false;
        }
    }
}


//==============================================================================
const juce::String Squwbs4AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Squwbs4AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Squwbs4AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Squwbs4AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Squwbs4AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Squwbs4AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Squwbs4AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Squwbs4AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Squwbs4AudioProcessor::getProgramName (int index)
{
    return {};
}

void Squwbs4AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Squwbs4AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    eq1.setSampleRate(sampleRate);
    eq2.setSampleRate(sampleRate);
    lpl.setSampleRate(sampleRate);
    lpr.setSampleRate(sampleRate);
    doubler.setSampleRate(sampleRate);
    limiterl.setSampleRate(sampleRate);
    limiterr.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;

    lowPassLeft.prepare(spec);
    lowPassRight.prepare(spec);
    lowPassLeft.reset();
    lowPassRight.reset();
    advLimiterL.prepareToPlay(sampleRate, samplesPerBlock);
    advLimiterR.prepareToPlay(sampleRate, samplesPerBlock);

}

void Squwbs4AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Squwbs4AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Squwbs4AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float mixValue = gainParameter != nullptr ? gainParameter->load() : 0.5f;
        const float volValue = volParameter != nullptr ? volParameter->load() : 1.0f;
        const float widthValue = widthParameter != nullptr ? widthParameter->load() : 0.5f;
        if (widthValue < 0.5f)
        {
            midSide.setRatio(widthValue);
            doubler.setMix(0.0f);
        }
        if (widthValue >= 0.5f)
        {
            midSide.setRatio(0.5f);
            doubler.setMix((widthValue-0.5f));
        }
        float currentMix = juce::jlimit (0.0f, 1.0f, mixValue);
        if (currentMix <= 0.5f)
            skewedMixFloat = currentMix * 1.5f;
        else
            skewedMixFloat = 0.75f + (currentMix - 0.5f) * 0.5f;

        skewedMixFloat = juce::jlimit (0.0f, 1.0f, skewedMixFloat);
        prevGain = skewedMixFloat;

        const float left = numChannels > 0 ? buffer.getSample (0, sample) : 0.0f;
        const float right = numChannels > 1 ? doubler.process(buffer.getSample (1, sample)) : doubler.process(left);
        

        //const float widthAmount = widthValue;
        
        const float wetLeft = ((left) * skewedMixFloat);
        const float wetRight =  ((right) * skewedMixFloat);
        const float* tempWet=eq1.match(wetLeft,wetRight);
        const float* wet=midSide.process(tempWet[0],tempWet[1]);
        const float processedLeft = advLimiterL.processSample (juce::jlimit (-1.0f, 1.0f,((left * (1.0f - skewedMixFloat)) + wet[0] * 48.0f) * volValue));
        const float processedRight = advLimiterR.processSample (juce::jlimit (-1.0f, 1.0f, ((right * (1.0f - skewedMixFloat)) + wet[1] * 48.0f) * volValue));

        buffer.setSample (0, sample, processedLeft);
        if (numChannels > 1)
            buffer.setSample (1, sample, processedRight);
        else
            buffer.setSample (0, sample, (processedLeft + processedRight) * 0.5f);
    }
}

//==============================================================================
bool Squwbs4AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Squwbs4AudioProcessor::createEditor()
{
    return new Squwbs4AudioProcessorEditor (*this);
}

//==============================================================================
void Squwbs4AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    auto xml = state.createXml();

    if (xml != nullptr)
    {
        copyXmlToBinary(*xml, destData);
    }
}

void Squwbs4AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);

        if (state.isValid())
        {
            parameters.replaceState(state);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Squwbs4AudioProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout Squwbs4AudioProcessor::createParameterLayout()
{
    // You can use a braced-list initializer for a clean definition
    /*
    return {
        
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("GAIN_ID", 1),    // Parameter ID and version
            "DRY/WET",                             // Display name
            juce::NormalisableRange<float>(0.0f, 1.0f,0.01f),// Range
            0.5f                                // Default value
        )
         
        // Create a vector to hold the parameters
            
    };
    */
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Example 1: Standard Linear Float Parameter (Gain)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "GAIN_ID", 1 },     // Parameter ID and version
        "DRY/WET",                              // Parameter Name (UI visible)
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), // min, max, step
        0.5f                                 // Default value
    ));

    // Example 2: Logarithmic Float Parameter (Filter Cutoff Frequency)
    // Using a skew factor of 0.3f to give more physical knob resolution to lower frequencies
    
    
    /*
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "LP_ID", 1 },
        "LOWPASS",
        juce::NormalisableRange<float> (20.0f, 20000.0f, 1.0f),
        20000.0f
    ));
    */
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "SKIN_ID", 1 },
        "SKIN",
        true
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "CHARACTER_ID", 1 },
        "CHARACTER",
        false
    ));

    // Example 3: Boolean Parameter (Bypass Switch)
    juce::NormalisableRange<float> volumeRange(0.0f,9.0f,0.01f);
    volumeRange.setSkewForCentre(1.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "VOL_ID", 1 },
        "VOLUME",
        volumeRange,
        1.0f                               // Default value (false = not bypassed)
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "WIDTH_ID", 1 },
        "WIDTH",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), // min, max, step
        0.5f                               // Default value (false = not bypassed)
    ));

    

    // Move the vector into the ParameterLayout object and return it
    return { params.begin(), params.end() };

}
