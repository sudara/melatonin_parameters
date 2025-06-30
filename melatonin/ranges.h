#pragma once


static inline juce::NormalisableRange<float> linearRange (const float start, const float end)
{
    return {
        start, end,
        // In all the following, "start" and "end" describe the unnormalized range
        // for example 0 to 15 or 0 to 100.
        [=] (const float, const float, const float normalised) {
            return start + normalised * (end - start);
        },
        [=] (const float, const float, const float unnormalised) {
            return (unnormalised - start) / (end - start);
        }
    };
}

// https://www.desmos.com/calculator/qkc6naksy5
//
// The exponent controls how exponential the curve is,
// Around a value of 6, halfway on a knob is about .15
// This value is "k" in the desmos link
static inline juce::NormalisableRange<float> logarithmicRange (const float logStart, const float logEnd, const float exponent = 6.0f)
{
    return {
        logStart, logEnd,
        // In all the following, "start" and "end" describe the unnormalized range
        // for example 0 to 15 or 0 to 100.
        [=] (const float start, const float end, const float normalised) {
            return start + (std::exp2 (normalised * exponent) - 1) * (end - start) / (std::exp2 (exponent) - 1);
        },
        [=] (const float start, const float end, const float unnormalised) {
            return std::log2 (((unnormalised - start) / (end - start) * (std::exp2 (exponent) - 1)) + 1) / exponent;
        }
    };
}

// sometimes you want a slider to start slower
// for example see lots of detail in the 0-30ms range of a 20-second slider
// this will basically plop a linear range in front of a compressed exponential range, bunching the exponential range up
// https://www.desmos.com/calculator/lz92tpns3b
static inline juce::NormalisableRange<float> logarithmicRangeWithLinearStart (const float logStart, const float logEnd, const float exponent, const float unnormalizedBreakpoint, const float breakpointOnSlider=0.25f)
{
    jassert (unnormalizedBreakpoint >= logStart);

    return {
        logStart, logEnd,
        // In all the following, "start" and "end" describe the unnormalized range
        // for example 0 to 15 or 0 to 100.
        [=] (const float start, const float end, const float normalizedValue) {

            if (normalizedValue < breakpointOnSlider)
            {
                // how close are we to the inflection point * available linear range
                return (normalizedValue / breakpointOnSlider) * (unnormalizedBreakpoint - start);
            }
            const auto normalizedX = (normalizedValue - breakpointOnSlider)/(1.0f - breakpointOnSlider);
            return unnormalizedBreakpoint + (std::exp2 (normalizedX * exponent) - 1) * (end - unnormalizedBreakpoint) / (std::exp2 (exponent) - 1);
        },
        [=] (const float, const float end, const float unnormalizedValue) {
            if (unnormalizedValue < unnormalizedBreakpoint)
            {
                return (unnormalizedValue / unnormalizedBreakpoint) * breakpointOnSlider;
            }
            return breakpointOnSlider + (1.0f - breakpointOnSlider) * std::log2 (((unnormalizedValue - unnormalizedBreakpoint) / (end - unnormalizedBreakpoint) * (std::exp2 (exponent) - 1)) + 1) / exponent;
        }
    };
}

static inline juce::NormalisableRange<float> reversedLogarithmicRange (float logStart, float logEnd, float exponent = 6.0f)
{
    return {
        logStart, logEnd,
        // In all the following, "start" and "end" describe the unnormalized range
        // for example 0 to 15 or 0 to 100.
        [=] (const float start, const float end, const float normalised) {
            return start + (std::exp2 ((1.0 - normalised) * exponent) - 1) * (end - start) / (std::exp2 (exponent) - 1);
        },
        [=] (const float start, const float end, float unnormalised) {
            return 1.0f - std::log2 (((unnormalised - start) / (end - start) * (std::exp2 (exponent) - 1)) + 1) / exponent;
        }
    };
}

static inline juce::NormalisableRange<float> intRangeWithMidPoint (int min, int max, int midpoint)
{
    const float linearSkew = 2 * (((float) midpoint - (float) min) / ((float) max - (float) min));

    auto range = juce::NormalisableRange<float> {
        (float) min, (float) max,

        // from0to1
        [=] (const float start, const float end, const float normalized) {
            if (normalized <= 0.5)
                return juce::jlimit (start, end, normalized * linearSkew * (end - start) + start);
            else
                return juce::jlimit (start, end, (float) midpoint + (normalized - 0.5f) * 2 * (end - (float) midpoint));
        },

        // to0to1
        [=] (const float start, const float end, const float unnormalized) {
            if (unnormalized <= (float) midpoint)
            {
                return juce::jlimit (0.0f, 1.0f, (unnormalized - start) / ((float) midpoint - start) / 2);
            }
            else
            {
                return juce::jlimit (0.0f, 1.0f, 0.5f + (unnormalized - (float) midpoint) / (end - (float) midpoint) / 2.0f);
            }
        },

        // snap
        [] (const float start, const float end, const float v) { return (float) juce::roundToInt (juce::jlimit (start, end, v)); }
    };

    range.setSkewForCentre ((float) midpoint);
    range.interval = 1.0f;
    return range;
}

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
static inline juce::NormalisableRange<float> decibelRangeForHarmonic (size_t harmonicNumber, float minimum = -100.f)
{
    jassert (minimum < 0.0f);
    float maxGainForHarmonic = 1.0f / (float) harmonicNumber;
    return {
        minimum,
        // The max gain for any given harmonic is 1/f
        juce::Decibels::gainToDecibels (maxGainForHarmonic, minimum),

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
    return decibelRangeForHarmonic (1);
}

static inline juce::NormalisableRange<float> decibelRange (float minimum, float maximum)
{
    jassert (minimum < 0.0f);

    return {
        minimum,
        maximum,
        [=] (float min, float max, float normalized) {
            auto minAmp = std::pow (10.0f, min / 20.0f);
            auto maxAmp = std::pow (10.0f, max / 20.0f);
            auto amp    = minAmp * std::pow (maxAmp / minAmp, normalized);
            return 20.0f * std::log10 (amp);
        },
        [=] (float min, float max, float dB) {
            auto minAmp = std::pow (10.0f, min / 20.0f);
            auto maxAmp = std::pow (10.0f, max / 20.0f);
            auto amp    = std::pow (10.0f, dB / 20.0f);
            return std::log (amp / minAmp) / std::log (maxAmp / minAmp);
        }
    };
}
