#pragma once
#include <JuceHeader.h>

class OverlappingBigKnob : public juce::Slider
{
public:
    // Pass your filmstrip image and the number of frames it contains
    OverlappingBigKnob(const void* imageData, int imageDataSize, int numberOfFrames)
        : numFrames(numberOfFrames)
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        knobStrip = juce::ImageCache::getFromMemory(imageData, imageDataSize);
        // Calculate single frame dimensions based on vertical or horizontal strip
        frameHeight = knobStrip.getHeight() / numFrames;
        frameWidth = knobStrip.getWidth();
    }

    ~OverlappingBigKnob() override = default;

    // 🎯 Hit-test based on the transparency of the current active frame pixel
    bool hitTest(int x, int y) override
    {
        if (knobStrip.isNull() || frameWidth <= 0 || frameHeight <= 0)
            return false;

        int width = getWidth();
        int height = getHeight();
        if (width <= 0 || height <= 0)
            return false;

        // 1. Determine which frame is currently visible
        int currentFrame = 0;
        if (getMaximum() > 0.0)
            currentFrame = juce::jlimit(0, numFrames - 1,
                static_cast<int>(std::floor(getValue() / getMaximum() * (numFrames - 1) + 0.5f)));

        // 2. Map the local mouse coordinates into the image frame
        int sourceX = juce::jlimit(0, frameWidth - 1, x * frameWidth / width);
        int sourceY = juce::jlimit(0, frameHeight - 1, y * frameHeight / height);
        sourceY += currentFrame * frameHeight;

        // 3. Use the image alpha to determine whether the hit is inside the visible knob shape
        juce::Colour pixelColor = knobStrip.getPixelAt(sourceX, sourceY);
        return pixelColor.getFloatAlpha() > 0.1f;
    }

    void paint(juce::Graphics& g) override
    {
        if (knobStrip.isValid())
        {
            int currentFrame = juce::jlimit(0, numFrames - 1, 
                static_cast<int>(std::ceil(getValue() / getMaximum() * (numFrames - 1))));

            // Draw only the portion of the strip representing the current frame
            g.drawImage(knobStrip,
                        0, 0, getWidth(), getHeight(),                 // Destination
                        0, currentFrame * frameHeight, frameWidth, frameHeight); // Source
        }
    }

private:
    juce::Image knobStrip;
    int numFrames = 1;
    int frameWidth = 0;
    int frameHeight = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlappingBigKnob)
};
