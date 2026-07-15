/*
  ==============================================================================

    secretOverlay.h
    Created: 15 Jul 2026 8:49:17am
    Author:  Bernard Ahn

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class secretOverlay : public juce::Component
{
public:
    secretOverlay()
    {
        setInterceptsMouseClicks(false, false);
        setWantsKeyboardFocus(false);

        // 1. Load your asset images (Ensure these are added via Projucer BinaryData or loaded from file)
        girlImage = juce::ImageCache::getFromMemory(BinaryData::character_png, BinaryData::character_pngSize);
        
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble00_png, BinaryData::speech_bubble00_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble01_png, BinaryData::speech_bubble01_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble02_png, BinaryData::speech_bubble02_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble03_png, BinaryData::speech_bubble03_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble04_png, BinaryData::speech_bubble04_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble05_png, BinaryData::speech_bubble05_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble06_png, BinaryData::speech_bubble06_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble07_png, BinaryData::speech_bubble07_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble08_png, BinaryData::speech_bubble08_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble09_png, BinaryData::speech_bubble09_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble10_png, BinaryData::speech_bubble10_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble11_png, BinaryData::speech_bubble11_pngSize));
        bubbleImages.add(juce::ImageCache::getFromMemory(BinaryData::speech_bubble12_png, BinaryData::speech_bubble12_pngSize));
        // Add as many pre-drawn speech bubble PNGs as you have...
    }

    ~secretOverlay() override = default;

    // Triggered by your Parameter Attachment or Listener
    void triggerSecretEasterEgg(bool shouldShow)
    {
        if (shouldShow && !isEasterEggActive)
        {
            // Pick a random bubble index if we have bubbles available
            if (bubbleImages.size() > 0)
            {
                currentBubbleIndex = random.nextInt(bubbleImages.size());
            }
        }
        
        isEasterEggActive = shouldShow;
        repaint(); // Force UI thread to redraw the component
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Layer 1: Draw the character image (scaled to fit the component bounds smoothly)
       

        // Layer 2: Overlay the random speech bubble if the secret parameter is found
        if (isEasterEggActive && currentBubbleIndex >= 0 && currentBubbleIndex < bubbleImages.size())
        {
            if (girlImage.isValid())
            {
                g.drawImage(girlImage, bounds, juce::RectanglePlacement::fillDestination);
            }
            const auto& selectedBubble = bubbleImages[currentBubbleIndex];
            
            if (selectedBubble.isValid())
            {
                g.drawImage(selectedBubble, bounds, juce::RectanglePlacement::fillDestination);
            }
        }
    }

    void resized() override
    {
        // Handle inner positioning if using separate child components instead of raw graphics drawing
    }
public:
    bool isEasterEggActive = false;
private:
    juce::Image girlImage;
    juce::Array<juce::Image> bubbleImages;
    
    
    int currentBubbleIndex = -1;
    
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(secretOverlay)
};
