#if RUN_MELATONIN_TESTS

    #include "melatonin_parameters.h"
    #include <catch2/catch_all.hpp>
    #include <juce_core/juce_core.h>

TEST_CASE ("logarithmicRange 0 to 1 with default curve", "[parameters]")
{
    auto range = logarithmicRange (0.0f, 1.0f);

    SECTION ("handles 0 perfectly")
    {
        REQUIRE (range.convertTo0to1 (0.0f) == 0.0f);
        REQUIRE (range.convertFrom0to1 (0.0f) == 0.0f);
    }

    SECTION ("handles 1 perfectly")
    {
        REQUIRE (range.convertTo0to1 (1.0f) == 1.0f);
        REQUIRE (range.convertFrom0to1 (1.0f) == 1.0f);
    }

    SECTION ("converts a linear range to logarithmic")
    {
        REQUIRE (range.convertFrom0to1 (0.1f) == Catch::Approx (0.00819f).margin (1e-5));
        REQUIRE (range.convertFrom0to1 (0.2f) == Catch::Approx (0.02059f).margin (1e-5));
        REQUIRE (range.convertFrom0to1 (0.5f) == Catch::Approx (0.11111f).margin (1e-5));
        REQUIRE (range.convertFrom0to1 (0.9f) == Catch::Approx (0.65435f).margin (1e-5));
    }

    SECTION ("converts a logarithmic range to linear")
    {
        REQUIRE (range.convertTo0to1 (0.01f) == Catch::Approx (0.11748f));
        REQUIRE (range.convertTo0to1 (0.05f) == Catch::Approx (0.34219f).margin (1e-5));
        REQUIRE (range.convertTo0to1 (0.1f) == Catch::Approx (0.47798f));
        REQUIRE (range.convertTo0to1 (0.2f) == Catch::Approx (0.62759f));
        REQUIRE (range.convertTo0to1 (0.5f) == Catch::Approx (0.83706f));
        REQUIRE (range.convertTo0to1 (0.9f) == Catch::Approx (0.97508f));
    }

    SECTION ("converting to and from results in the original value (no drifting or feedback loop)")
    {
        REQUIRE (range.convertFrom0to1 (range.convertTo0to1 (0.01f)) == Catch::Approx (0.01f));
        REQUIRE (range.convertTo0to1 (range.convertFrom0to1 (0.01f)) == Catch::Approx (0.01f));

        REQUIRE (range.convertFrom0to1 (range.convertTo0to1 (0.1f)) == Catch::Approx (0.1f));
        REQUIRE (range.convertTo0to1 (range.convertFrom0to1 (0.1f)) == Catch::Approx (0.1f));

        REQUIRE (range.convertFrom0to1 (range.convertTo0to1 (0.5f)) == Catch::Approx (0.5f));
        REQUIRE (range.convertTo0to1 (range.convertFrom0to1 (0.5f)) == Catch::Approx (0.5f));

        REQUIRE (range.convertFrom0to1 (range.convertTo0to1 (0.9f)) == Catch::Approx (0.9f));
        REQUIRE (range.convertTo0to1 (range.convertFrom0to1 (0.9f)) == Catch::Approx (0.9f));
    }
}

TEST_CASE ("logarithmicRange 0 to 44100 with exponent of 10", "[parameters]")
{
    auto range = logarithmicRange (0.0f, 44100.0f, 10);

    SECTION ("handles 0 perfectly")
    {
        REQUIRE (range.convertTo0to1 (0.0f) == 0.0f);
        REQUIRE (range.convertFrom0to1 (0.0f) == 0.0f);
    }

    SECTION ("handles 44100 perfectly")
    {
        REQUIRE (range.convertTo0to1 (44100.0f) == 1.0f);
        REQUIRE (range.convertFrom0to1 (1.0f) == 44100.0f);
    }

    SECTION ("converts happily")
    {
        //REQUIRE(range.convertTo0to1(441.0f) == 0.1f);
        REQUIRE (range.convertFrom0to1 (0.1f) == Catch::Approx (43.10851f));
        REQUIRE (range.convertFrom0to1 (0.5f) == Catch::Approx (1336.3636f));

        REQUIRE (range.convertTo0to1 (441.f) == Catch::Approx (0.34893f));
        REQUIRE (range.convertTo0to1 (4410.f) == Catch::Approx (0.66907f));
    }
}
TEST_CASE ("decibelRange", "[parameters]")
{
    SECTION ("unnormalized db values range from -100 to 0")
    {
        auto range = decibelRange();
        REQUIRE (range.convertFrom0to1 (0.0f) == -100.f);
        REQUIRE (range.convertFrom0to1 (1.0f) == 0.0f);

        REQUIRE (range.convertTo0to1 (-100.f) == 0.0f);
        REQUIRE (range.convertTo0to1 (0.0f) == 1.0f);
    }

    SECTION ("handles out of bounds input")
    {
        auto range = decibelRange();
        REQUIRE (range.convertTo0to1 (-120.f) == 0.0f);
    }

    //  See docs/Per harmonic db ranges.numbers for example values
    SECTION ("for a harmonic")
    {
        auto range2 = decibelRange (2);
        auto range3 = decibelRange (3);
        auto range10 = decibelRange (10);

        SECTION ("caps the max gain at 1/f, where f is the harmonic number")
        {
            REQUIRE (range2.convertFrom0to1 (1.0f) == Catch::Approx (juce::Decibels::gainToDecibels(0.5f)));
            REQUIRE (range3.convertFrom0to1 (1.0f) == Catch::Approx (juce::Decibels::gainToDecibels(0.3333333f)));
            REQUIRE (range10.convertFrom0to1 (1.0f) == Catch::Approx (juce::Decibels::gainToDecibels(0.1f)));
        }

        SECTION ("uses the full normalized range of 0 to 1 for the harmonic")
        {
            REQUIRE (range2.convertTo0to1 (-100.f) == 0.0f);
            REQUIRE (range2.convertTo0to1 (-6.0206f) == Catch::Approx (1.0f));

            REQUIRE (range3.convertTo0to1 (-100.f) == 0.0f);
            REQUIRE (range3.convertTo0to1 (-9.5424f) == Catch::Approx (1.0f));
        }

        SECTION ("normalized range results in unique db scale per harmonic")
        {
            REQUIRE (range2.convertFrom0to1 (1.0f) == Catch::Approx (-6.0206));
            REQUIRE (range2.convertFrom0to1 (0.5f) == Catch::Approx (-12.0412f));
            REQUIRE (range2.convertFrom0to1 (0.1f) == Catch::Approx (-26.0206f));
            REQUIRE (range2.convertFrom0to1 (0.01f) == Catch::Approx (-46.021f));

            REQUIRE (range3.convertFrom0to1 (1.0f) == Catch::Approx (-9.54243f));
            REQUIRE (range3.convertFrom0to1 (0.5f) == Catch::Approx (-15.563f));
            REQUIRE (range3.convertFrom0to1 (0.1f) == Catch::Approx (-29.54243f));
            REQUIRE (range3.convertFrom0to1 (0.01f) == Catch::Approx (-49.54243f));
        }
    }
}

TEST_CASE ("timeValueFromString", "[parameters]")
{
    SECTION ("converts ms correctly")
    {
        REQUIRE (timeValueFromString (".1ms") == Catch::Approx (0.0001f));
        REQUIRE (timeValueFromString ("0.1ms") == Catch::Approx (0.0001f));
        REQUIRE (timeValueFromString ("1ms") == 0.001f);
        REQUIRE (timeValueFromString ("10ms") == Catch::Approx (0.01f));
        REQUIRE (timeValueFromString ("100ms") == 0.1f);
        REQUIRE (timeValueFromString ("1000ms") == 1.0f);
    }

    SECTION ("converts s correctly")
    {
        REQUIRE (timeValueFromString (".1s") == 0.1f);
        REQUIRE (timeValueFromString ("0.1s") == 0.1f);
        REQUIRE (timeValueFromString ("1s") == 1.0f);
        REQUIRE (timeValueFromString ("10s") == 10.0f);
    }

    SECTION ("assumes s when no units single digit and decimal")
    {
        REQUIRE (timeValueFromString ("1.0") == 1.0f);
        REQUIRE (timeValueFromString ("3.5") == 3.5f);
        REQUIRE (timeValueFromString ("5.0") == 5.0f);
    }

    SECTION ("assumes ms when no units specified and no decimal")
    {
        REQUIRE (timeValueFromString ("1") == 0.001f);
        REQUIRE (timeValueFromString ("10") == Catch::Approx (0.01f));
        REQUIRE (timeValueFromString ("100") == 0.1f);
        REQUIRE (timeValueFromString ("1000") == 1.0f);
    }
}

TEST_CASE ("stringFromTimeValue", "[parameters]")
{
    SECTION ("returns ms under 1s, removes 0s from decimal")
    {
        REQUIRE (stringFromTimeValue (0.f) == "0ms");
        REQUIRE (stringFromTimeValue (0.001f) == "1ms");
        REQUIRE (stringFromTimeValue (0.01f) == "10ms");
        REQUIRE (stringFromTimeValue (0.1f) == "100ms");
        REQUIRE (stringFromTimeValue (0.499f) == "499ms");
    }

    SECTION ("returns s over .5s, always has 2 digits of precision")
    {
        REQUIRE (stringFromTimeValue (0.5f) == "0.50s");
        REQUIRE (stringFromTimeValue (0.6f) == "0.60s");
        REQUIRE (stringFromTimeValue (1.0f) == "1.00s");
        REQUIRE (stringFromTimeValue (1.1f) == "1.10s");
        REQUIRE (stringFromTimeValue (1.11f) == "1.11s");
    }

    // the second argument will be ignored in our implementation
    // but is required for juce::AudioParameterFloat's constructor
    SECTION ("returns exactly two digits of precision on seconds")
    {
        REQUIRE (stringFromTimeValue (1.f, 6) == "1.00s");
        REQUIRE (stringFromTimeValue (1.1f, 5) == "1.10s");

        REQUIRE (stringFromTimeValue (1.11111f, 5) == "1.11s");
        REQUIRE (stringFromTimeValue (1.11111f, 6) == "1.11s");

        REQUIRE (stringFromTimeValue (11.11111f, 6) == "11.11s");
        REQUIRE (stringFromTimeValue (11.11111f, 10) == "11.11s");

        REQUIRE (stringFromTimeValue (111.11111f, 10) == "111.11s");
        REQUIRE (stringFromTimeValue (111.11111f, 10) == "111.11s");
    }

    SECTION ("returns no digit of precision on ms")
    {
        REQUIRE (stringFromTimeValue (0.11111f, 5) == "111ms");
        REQUIRE (stringFromTimeValue (0.11111f, 6) == "111ms");
        REQUIRE (stringFromTimeValue (0.11111f, 7) == "111ms");
    }
}

TEST_CASE ("stringFromDBValue", "[parameters]")
{
    SECTION ("Lowest value is -100db")
    {
        REQUIRE (stringFromDBValue (-100.0f) == "-100.0db");
    }

    SECTION ("Highest value is -0db")
    {
        REQUIRE (stringFromDBValue (0.0f) == "0.0db");
    }

    SECTION ("Converts to db, max 1 decimal places")
    {
        REQUIRE (stringFromDBValue (-6.123f) == "-6.1db");
    }
}

TEST_CASE ("dBFromString", "[parameters]")
{
    SECTION ("-100db converts to 0.0f")
    {
        REQUIRE (dBFromString ("-100db") == -100.0f);
        REQUIRE (dBFromString ("-100.00db") == -100.0f);
    }

    SECTION ("0db converts to 1.0f")
    {
        REQUIRE (dBFromString ("0db") == 0.0f);
        REQUIRE (dBFromString ("0.0db") == 0.0f);
    }

    SECTION ("Converts to db when db unit label specified")
    {
        REQUIRE (dBFromString ("-3db") == -3.0f);
    }

    SECTION ("Converts from db even when unit label not specified (no db written at end)")
    {
        REQUIRE (dBFromString ("-3") == -3.0f);
        REQUIRE (dBFromString ("-3.1") == -3.1f);
        REQUIRE (dBFromString ("-3.12") == -3.12f);
    }
}

#endif
