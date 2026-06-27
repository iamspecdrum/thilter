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
        int sourceY = static_cast<int>(y * scaleY);

        // 3. Clamp to valid image coordinates within a single frame
        sourceX = juce::jlimit(0, frameWidth - 1, sourceX);
        sourceY = juce::jlimit(0, frameHeight - 1, sourceY);

        // Convert to coordinates within the full strip when sampling the pixel
        int pixelX = sourceX;
        int pixelY = currentFrame * frameHeight + sourceY;

        // 4. (optional) Extract the pixel color if you want transparency-based hit-testing
        // juce::Colour pixelColor = knobStrip.getPixelAt(pixelX, pixelY);

        // Test against the desired rectangle in frame-local coordinates
        bool xIn = sourceX > 74 && sourceX < 264;
        bool yIn = sourceY > 32 && sourceY < 222;
        bool allIn = xIn && yIn;

        return allIn;
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
