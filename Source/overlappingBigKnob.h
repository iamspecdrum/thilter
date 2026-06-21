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
        if (knobStrip.isNull())
            return false;

        // 1. Determine which frame is currently visible
        int currentFrame = juce::jlimit(0, numFrames - 1, 
            static_cast<int>(std::ceil(getValue() / getMaximum() * (numFrames - 1))));

        // 2. Scale the component's mouse coordinates to the source image coordinates
        // The component is drawn at (0, 0, getWidth(), getHeight())
        // But the source image is (frameWidth, frameHeight)
        float scaleX = static_cast<float>(frameWidth) / getWidth();
        float scaleY = static_cast<float>(frameHeight) / getHeight();
        
        int sourceX = static_cast<int>(x * scaleX);
        int sourceY = (currentFrame * frameHeight) + static_cast<int>(y * scaleY);

        // 3. Clamp to valid image coordinates
        sourceX = juce::jlimit(0, frameWidth - 1, sourceX);
        sourceY = juce::jlimit(currentFrame * frameHeight, (currentFrame + 1) * frameHeight - 1, sourceY);

        // 4. Extract the pixel color and check the alpha channel
        juce::Colour pixelColor = knobStrip.getPixelAt(sourceX, sourceY);
        
        // Return true only if the pixel is mostly opaque (alpha > 0.1)
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
