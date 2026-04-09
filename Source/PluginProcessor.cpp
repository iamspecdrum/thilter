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
}

Squwbs4AudioProcessor::~Squwbs4AudioProcessor()
{
    parameters.removeParameterListener("GAIN_ID",this);
}
void Squwbs4AudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if(parameterID == "GAIN_ID")
    {
        juce::ignoreUnused(newValue);
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
    juce::ignoreUnused(midiMessages);
    
    //const float currentGain = gainParameter->load();
    //if (currentGain != previousGain){
    //    buffer.applyGainRamp(0,0.05,previousGain,currentGain);
    //}
    //else{
    //    buffer.applyGain(currentGain);
    //}
    
    juce::AudioSampleBuffer main = getBusBuffer(buffer,true,0);
    if (main.getNumChannels() == 1)
    {
        
        for (int j = 0; j<main.getNumSamples(); ++j)
        {
            
            mixFloat = gainParameter->load()/100.0;
            //std::cout<<mixFloat<<std::endl;
            
            if (mixFloat<=0.5){
                skewedMixFloat = mixFloat*3/2;
            }
            else{
                skewedMixFloat = 0.75+(mixFloat-0.5)/2.0;
            }
            float increment = (skewedMixFloat-prevGain)/float(main.getNumSamples());
            float left = main.getSample(0, j);
            float right = main.getSample(0, j);

            if(skewedMixFloat==prevGain){
                main.setSample(0, j, eq1.match(left,right)[0]*64.0*skewedMixFloat+left*(1.0-skewedMixFloat));
                main.setSample(1, j, eq2.match(left,right)[1]*64.0*skewedMixFloat+right*(1.0-skewedMixFloat));
                //main.setSample(0, j, eq1.match(left,right)[0]);
                //main.setSample(1, j, eq1.match(left,right)[1]);
                //main.setSample(0, j, eq1.match(left,right)[0]*64.0*skewedMixFloat);
                //main.setSample(1, j, eq2.match(left,right)[1]*64.0*skewedMixFloat);
            }
            else{
                prevGain=prevGain+increment;
                main.setSample(0, j, eq1.match(left,right)[0]*64.0*prevGain+left*(1.0-prevGain));
                main.setSample(1, j, eq2.match(left,right)[1]*64.0*prevGain+right*(1.0-prevGain));
            }
        }
    }
        
        
    
    else if (main.getNumChannels() == 2)
    {

        for (int j = 0; j<main.getNumSamples(); ++j)
        {
            
            mixFloat = gainParameter->load();
            //std::cout<<mixFloat<<std::endl;
            if (mixFloat<=0.5){
                skewedMixFloat = mixFloat*3/2;
            }
            else{
                skewedMixFloat = 0.75+(mixFloat-0.5)/2.0;
            }
            float increment = (skewedMixFloat-prevGain)/float(main.getNumSamples());
            float left = main.getSample(0, j);
            float right = main.getSample(1, j);
            if(skewedMixFloat==prevGain){
                main.setSample(0, j, eq1.match(left,right)[0]*64.0*skewedMixFloat+left*(1.0-skewedMixFloat));
                main.setSample(1, j, eq2.match(left,right)[1]*64.0*skewedMixFloat+right*(1.0-skewedMixFloat));
                
                
            }
            else{
                prevGain=prevGain+increment;
                main.setSample(0, j, eq1.match(left,right)[0]*64.0*prevGain+left*(1.0-prevGain));
                main.setSample(1, j, eq2.match(left,right)[1]*64.0*prevGain+right*(1.0-prevGain));
            }
        }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Squwbs4AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
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
    return {
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("GAIN_ID", 1),    // Parameter ID and version
            "DRY/WET",                             // Display name
            juce::NormalisableRange<float>(0.0f, 1.0f,0.01f),// Range
            0.5f                                // Default value
        )
    };
}
