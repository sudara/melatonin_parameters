#pragma once

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
static inline juce::NormalisableRange<float> logarithmicRangeWithLinearStart (const float logStart, const float logEnd, const float exponent, const float unnormalizedBreakpoint)
{
    jassert (unnormalizedBreakpoint >= logStart);

    // this can eventually be parameterized
    constexpr float breakpointOnSlider = 0.25f;

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
        [=] (const float start, const float end, const float unnormalizedValue) {
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
