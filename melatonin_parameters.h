/* 
BEGIN_JUCE_MODULE_DECLARATION

 ID:               melatonin_parameters
 vendor:           Sudara
 version:          1.0.0
 name:             Juce Parameter Helpers
 description:      Collection of range and string formatting helpers for JUCE
 license:          BSD
 dependencies:     juce_core, juce_audio_basics

END_JUCE_MODULE_DECLARATION
*/

#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>

// https://www.desmos.com/calculator/qkc6naksy5
//
// The exponent controls how exponential the curve is,
// Around a value of 6, halfway on a knob is about .15
// This value is "k" in the desmos link
static inline juce::NormalisableRange<float> logarithmicRange (float logStart, float logEnd, float exponent = 6.0f)
{
    return {
        logStart, logEnd,
        // In all the following, "start" and "end" describe the unnormalized range
        // for example 0 to 15 or 0 to 100.
        [=] (float start, float end, float normalised) {
            return start + (std::exp2 (normalised * exponent) - 1) * (end - start) / (std::exp2 (exponent) - 1);
        },
        [=] (float start, float end, float unnormalised) {
            return std::log2 (((unnormalised - start) / (end - start) * (std::exp2 (exponent) - 1)) + 1) / exponent;
        },
        [=] (float start, float end, float value) {
            return juce::jlimit (start, end, (float) juce::roundToInt (value * float (10000)) / float (10000));
        }
    };
}

// maximumStringLength is unused in this function
// but must stay in place as it's the required signature for juce::AudioParameterFloat
static inline auto stringFromTimeValue = [] (float value, [[maybe_unused]] int maximumStringLength = 5) {
    juce::String result;

    if ((value < 0.0) || (value == 0.0))
    {
        result = juce::String ("0ms");
    }
    else if (value < 0.5)
    {
        // We want 0 number of decimal places for ms values
        auto rawResult = juce::String (value * 1000, 1).dropLastCharacters (2);
        result = rawResult + "ms";
    }
    else
    {
        auto rawResult = juce::String (value, 2);
        result = rawResult + "s";
    }

    return result;
};

// The value can either be 0ms, 11.1ms, 100ms, 1.0s, 15.98s
// The values can also come in without labels
// In that case, single digits or a decimal place will trigger seconds conversion
static inline auto timeValueFromString = [] (const juce::String& text) {
    float value;
    if (text.endsWith ("ms"))
    {
        value = text.dropLastCharacters (2).getFloatValue() / 1000.0f;
    }
    else if (text.endsWith ("s"))
    {
        value = text.dropLastCharacters (1).getFloatValue();
    }
    else if (text.contains ("."))
    {
        value = text.getFloatValue();
    }
    else
    {
        value = text.getFloatValue() / 1000.0f;
    }
    return value;
};

static inline auto stringFromDBValue = [] (float value, [[maybe_unused]] int maximumStringLength = 5) {
    // only 1 decimal place for db values
    return juce::String (value, 1) + "db";
};

static inline auto dBFromString = [] (const juce::String& text) {
    if (text.endsWith ("db"))
    {
        return text.dropLastCharacters (2).getFloatValue();
    }
    else
        return text.getFloatValue();
};

/* This creates a range for a particular harmonic
 *
 * This stuff can sometimes feel a bit tricky or blurry, here are some tips:
 *  * The normalized range of 0-1 is always the same, think of it as "the full range of a knob"
 *  * What changes is the *output*, which is also what's stored by JUCE as the parameter value
 *  * This output is then fed through the optional string functions.
 *
 *  You could do further conversions via the string functions, for example, to provide 2 representations to the user.
 *
 *  https://forum.juce.com/t/decibels-in-normalisablerange-using-lambdas/26379/6
 */
static inline juce::NormalisableRange<float> decibelRange (size_t harmonicNumber)
{
    float maxGainForHarmonic = 1.0f / (float) harmonicNumber;
    return {
        -100.0f,
        // The max gain for any given harmonic is 1/f
        juce::Decibels::gainToDecibels (maxGainForHarmonic, -100.0f),

        // convertFrom0to1
        [=] (float min, float max, float normalizedGain) {
            return juce::Decibels::gainToDecibels<float> (normalizedGain / (float) harmonicNumber, min);
        },

        // convertTo0to1
        [=] (float min, float max, float dB) {
            // This can sometimes result in a number just barely above 1.0f
            return juce::jmin (1.0f, (float) harmonicNumber * juce::Decibels::decibelsToGain (dB, min));
        }
    };
}

// This is a generic gain <-> decibel range
static inline juce::NormalisableRange<float> decibelRange()
{
    return decibelRange (1);
}
