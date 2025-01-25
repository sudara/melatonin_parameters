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

TEST_CASE ("intRangeWithMidPoint")
{
    SECTION ("from 0 to 1")
    {
        SECTION ("returns the midpoint when the value is 0.5")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            REQUIRE (range.convertFrom0to1 (0.5f) == Catch::Approx(80.f));
        }

        SECTION ("returns the min when the value is 0")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            REQUIRE (range.convertFrom0to1 (0.0f) == 0.f);
        }

        SECTION ("returns the max when the value is 1")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            REQUIRE (range.convertFrom0to1 (1.0f) == 100.f);
        }

        SECTION ("linear distribution under midpoint")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            CHECK (range.convertFrom0to1 (0.1f) == Catch::Approx (16.f));
            CHECK (range.convertFrom0to1 (0.2f) == Catch::Approx (32.f));
            CHECK (range.convertFrom0to1 (0.3f) == Catch::Approx (48.f));
        }

        SECTION ("linear distribution above midpoint")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            CHECK (range.convertFrom0to1 (0.6f) == 84.f);
            CHECK (range.convertFrom0to1 (0.7f) == 88.f);
            CHECK (range.convertFrom0to1 (0.8f) == 92.f);
        }
    }

    SECTION ("to 0 to 1")
    {
        SECTION ("returns 0.5 when the value is the midpoint")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            REQUIRE (range.convertTo0to1 (80.f) == 0.5f);
        }

        SECTION ("returns 0 when the value is the min")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            REQUIRE (range.convertTo0to1 (0.f) == 0.0f);
        }

        SECTION ("returns 1 when the value is the max")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            REQUIRE (range.convertTo0to1 (100.f) == 1.0f);
        }

        SECTION ("linear distribution below midpoint")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            CHECK (range.convertTo0to1 (16.f) == Catch::Approx (0.1f));
            CHECK (range.convertTo0to1 (32.f) == Catch::Approx (0.2f));
            CHECK (range.convertTo0to1 (48.f) == Catch::Approx (0.3f));
        }

        SECTION ("linear distribution above midpoint")
        {
            auto range = intRangeWithMidPoint (0, 100, 80);
            CHECK (range.convertTo0to1 (84.f) == Catch::Approx (0.6f));
            CHECK (range.convertTo0to1 (88.f) == Catch::Approx (0.7f));
            CHECK (range.convertTo0to1 (92.f) == Catch::Approx (0.8f));
        }
    }
}

TEST_CASE ("Melatonin Parameters to/from string")
{
    SECTION ("stringFromTimeValue", "[parameters]")
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

    SECTION ("stringFromDBValue", "[parameters]")
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

    SECTION ("dBFromString", "[parameters]")
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

    SECTION ("stringFromIntValue")
    {
        SECTION ("returns the value as a string")
        {
            REQUIRE (stringFromIntValue (0.0000000000f) == "0");
            REQUIRE (stringFromIntValue (1.00000001f) == "1");
            REQUIRE (stringFromIntValue (2) == "2");
            REQUIRE (stringFromIntValue (10) == "10");
        }
    }

    SECTION ("intValueFromString")
    {
        SECTION ("returns the value as a string")
        {
            REQUIRE (intValueFromString ("0") == 0.f);
            REQUIRE (intValueFromString ("1") == 1.f);
            REQUIRE (intValueFromString ("2") == 2.f);
            REQUIRE (intValueFromString ("10") == 10.f);
        }
    }

    SECTION ("stringFromPercentageValue")
    {
        SECTION ("converts to a nicely formatted percentage")
        {
            REQUIRE (stringFromPercentValue (0.0000000000f) == "0%");
            REQUIRE (stringFromPercentValue (0.9999999f) == "100%");
            REQUIRE (stringFromPercentValue (1.00000001f) == "100%");
            REQUIRE (stringFromPercentValue (0.5f) == "50%");
            REQUIRE (stringFromPercentValue (0.25f) == "25%");
            REQUIRE (stringFromPercentValue (0.25555555f) == "25%");
        }
    }

    SECTION ("percentageValueFromString")
    {
        SECTION ("parses a percentage")
        {
            REQUIRE (percentValueFromString ("0%") == 0.f);
            REQUIRE (percentValueFromString ("100%") == 1.f);
            REQUIRE (percentValueFromString ("50%") == 0.5f);
            REQUIRE (percentValueFromString ("25%") == 0.25f);
        }
    }
}
