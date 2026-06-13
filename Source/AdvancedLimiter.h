
#pragma once

#include <JuceHeader.h>

class AdvancedLimiter
{
public:
    AdvancedLimiter() = default;
    ~AdvancedLimiter() = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources() {}
    float processSample (float inputSample);

private:
    float ceilingDb = -0.2f;
    float lookAheadMs = 1.5f;
    float releaseMs = 100.0f;

    double currentSampleRate = 48000.0;

    // Ring buffer for look-ahead processing.
    juce::AudioBuffer<float> delayBuffer;
    int writeIndex = 0;

    // Envelope-processing state.
    float envVal = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdvancedLimiter)
};
