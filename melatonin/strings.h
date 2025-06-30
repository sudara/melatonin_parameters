#pragma once

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

// make this accept float or double
static inline auto stringFromDBValueWithOffAt64 = [] (float value, [[maybe_unused]] int maximumStringLength = 5) {
    // only 1 decimal place for db values
    if (juce::approximatelyEqual (value, -64.0f))
        return juce::String ("OFF");
    return juce::String (value, 1) + "db";
};

static inline auto dBFromStringWithOffAt64 = [] (const juce::String& text) {
    if (text.toLowerCase() == "off")
        return -64.0f;
    if (text.endsWith ("db"))
    {
        return text.dropLastCharacters (2).getFloatValue();
    }
    else
        return text.getFloatValue();
};


static inline auto stringFromIntValue = [] (float value, [[maybe_unused]] int maximumStringLength = 5) {
    return juce::String ((int) value);
};

static inline auto intValueFromString = [] (const juce::String& text) {
    return (float) text.getIntValue();
};

static inline auto stringFromPercentValue = [] (float value, [[maybe_unused]] int maximumStringLength = 0) {
        // we want 0 significant digits, but this juce string helper is odd...
        // so we have to make one decimal place, then drop that and the decimal char
        return juce::String (value * 100.0f, 1).dropLastCharacters (2) + "%";
};

template<int MaxDigits>
static inline auto stringFromPercentValueWithDigits = [] (float value, [[maybe_unused]] int maximumStringLength = 0) {
    return juce::String (value * 100.0f, MaxDigits) + "%";
};

static inline auto percentValueFromString = [] (const juce::String& text) {
    if (text.endsWith ("%"))
    {
        return text.dropLastCharacters (1).getFloatValue() / 100.0f;
    }
    return text.getFloatValue() / 100.0f;
};

static inline auto stringFromHzValue = [] (float value, [[maybe_unused]] int maximumStringLength = 5) {
    if (value < 2.0f)
        return juce::String (value, 2) + "Hz";
    if (value < 10.0f)
        return juce::String (value, 1) + "Hz";

    return juce::String (value, 1).dropLastCharacters (2) + "Hz";
};

static inline auto hzValueFromString = [] (const juce::String& text) {
    if (text.endsWith ("Hz") || (text.endsWith ("hz")))
    {
        return text.dropLastCharacters (2).getFloatValue();
    }
    return text.getFloatValue();
};

static inline auto stringFrom0to1 = [] (float value, [[maybe_unused]] int maximumStringLength = 4) {
    return juce::String (value, maximumStringLength);
};

static inline auto zeroTo1FromString = [] (const juce::String& text) {
    return text.getFloatValue();
};
