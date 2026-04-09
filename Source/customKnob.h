/*
  ==============================================================================

    customKnob.h
    Created: 12 Oct 2025 6:07:24am
    Author:  Bernard Ahn

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"

class CustomRotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        // Load the image from BinaryData
        juce::Image filmStrip = juce::ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);

        // Check if the image was loaded correctly
        if (filmStrip.isValid())
        {
            // Get the total number of frames in the strip
            int numFrames = filmStrip.getHeight() / filmStrip.getWidth();
            
            // Calculate the frame index based on the slider's proportional value
            int frameIndex = static_cast<int>(sliderPosProportional * (numFrames - 1));

            // Create the source rectangle to draw the correct frame
            int sourceX = 0;
            int sourceY = frameIndex * filmStrip.getWidth();
            int sourceWidth = filmStrip.getWidth();
            int sourceHeight = filmStrip.getWidth();
            // Draw the frame onto the component's graphics context
            // The method will scale the image from the source rect to fit the dest rect.
            g.drawImage(filmStrip, x,y,width,height,sourceX,sourceY, sourceWidth,sourceHeight,false);
        }
    }
};
