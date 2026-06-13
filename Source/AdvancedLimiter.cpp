//
//  AdvancedLimiter.cpp
//  thilter
//
//  Created by Bernard Ahn on 6/13/26.
//  Copyright © 2026 squwbs. All rights reserved.
//

#include "AdvancedLimiter.h"

void AdvancedLimiter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    // Allocate a small look-ahead buffer (10 ms is enough for a limiter helper).
    const int maxDelaySamples = juce::jmax (1, static_cast<int> (sampleRate * 0.01));
    delayBuffer.setSize (1, maxDelaySamples);
    delayBuffer.clear();

    writeIndex = 0;
    envVal = 0.0f;
}

float AdvancedLimiter::processSample (float inputSample)
{
    juce::ScopedNoDenormals noDenormals;

    if (delayBuffer.getNumSamples() <= 0)
        return inputSample;

    const float sampleRate = static_cast<float> (currentSampleRate);
    const float ceiling = juce::Decibels::decibelsToGain (ceilingDb);
    const int lookAheadSamples = static_cast<int> (lookAheadMs * sampleRate / 1000.0f);
    const float releaseCoeff = std::exp (-1.0f / (sampleRate * (releaseMs / 1000.0f)));

    const float currentPeak = std::abs (inputSample);

    if (currentPeak > envVal)
        envVal = currentPeak;
    else
        envVal = envVal * releaseCoeff + currentPeak * (1.0f - releaseCoeff);

    float targetGain = 1.0f;
    if (envVal > 0.0f)
        targetGain = std::min (1.0f, ceiling / envVal);

    delayBuffer.getWritePointer (0)[writeIndex] = inputSample;

    int readIndex = writeIndex - lookAheadSamples;
    if (readIndex < 0)
        readIndex += delayBuffer.getNumSamples();

    const float delayedSample = delayBuffer.getReadPointer (0)[readIndex];

    writeIndex = (writeIndex + 1) % delayBuffer.getNumSamples();

    return delayedSample * targetGain;
}
