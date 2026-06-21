
#pragma once
#include <JuceHeader.h>


class SkinButton : public juce::Button
{
public:
   SkinButton(const juce::String& name, const char* data, int dataSize) : juce::Button(name)
    {
        imageStrip = juce::ImageFileFormat::loadFrom(data, static_cast<size_t>(dataSize));
        
        // Allow the button to toggle state on click so UI mode can reflect the current skin
        setClickingTogglesState(false);
    }

    void paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        if (imageStrip.isNull() || !imageStrip.isValid())
            return;

        int numStates = 3; // Off-Normal, Off-Over, On-Normal, On-Over
        
        // Use integers for source image math
        int frameHeight = imageStrip.getHeight() / numStates;
        int frameWidth = imageStrip.getWidth();

        int frameIndex = 0;
        bool isOn = getToggleState();

        if (!isOn)
        {
            //frameIndex = isMouseOverButton ? 1 : 0;
            frameIndex = 1;
        }
        else
        {
            //frameIndex = 2;
            frameIndex=1;
            i++;
            DBG(i)
        }

        // Explicitly define source as an integer rectangle
        juce::Rectangle<int> sourceRect(0, frameIndex * frameHeight, frameWidth, frameHeight);
        
        // Explicitly define destination as a float rectangle
        juce::Rectangle<float> destRect = getLocalBounds().toFloat();

        // Call the most universal, unambiguous JUCE drawImage overload
        g.drawImage(imageStrip,
                    destRect.getX(), destRect.getY(), destRect.getWidth(), destRect.getHeight(), // Target bounds
                    sourceRect.getX(), sourceRect.getY(), sourceRect.getWidth(), sourceRect.getHeight()); // Source bounds
    }

private:
     juce::Image imageStrip;
     int i = 0;
};
;
