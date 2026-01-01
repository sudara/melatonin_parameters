TEST_CASE ("Melatonin Parameters Ranges")
{
    SECTION ("linear range 0 to 1")
    {
        auto range = linearRange (0.0f, 1.0f);
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
    }

    SECTION ("linear range -1 to 1")
    {
        auto range = linearRange (-1.0f, 1.0f);
        SECTION ("handles -1 perfectly")
        {
            REQUIRE (range.convertTo0to1 (-1.0f) == 0.0f);
            REQUIRE (range.convertFrom0to1 (0.0f) == -1.0f);
        }

        SECTION ("handles 1 perfectly")
        {
            REQUIRE (range.convertTo0to1 (1.0f) == 1.0f);
            REQUIRE (range.convertFrom0to1 (1.0f) == 1.0f);
        }

        SECTION ("handles 0 perfectly")
        {
            REQUIRE (range.convertTo0to1 (0.0f) == 0.5f);
            REQUIRE (range.convertFrom0to1 (0.5f) == 0.0f);
        }
    }

    SECTION ("logarithmicRange 0 to 1 with default exponent of 6", "[parameters]")
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

    SECTION ("reversedLogarithimicRange 0 to 10 with default curve", "[parameters]")
    {
        auto range = reversedLogarithmicRange (0.0f, 10.0f);

        SECTION ("handles 0 perfectly")
        {
            REQUIRE (range.convertTo0to1 (0.0f) == 1.0f);
            REQUIRE (range.convertFrom0to1 (1.0f) == 0.0f);
        }

        SECTION ("handles 10 perfectly")
        {
            REQUIRE (range.convertTo0to1 (10.0f) == 0.0f);
            REQUIRE (range.convertFrom0to1 (0.0f) == 10.0f);
        }

        SECTION ("converts a linear to reversed logarithmic")
        {
            CHECK (range.convertFrom0to1 (0.9f) == Catch::Approx (0.0819f).margin (0.0001f));
            CHECK (range.convertFrom0to1 (0.8f) == Catch::Approx (0.2059f).margin (0.0001f));
            CHECK (range.convertFrom0to1 (0.5f) == Catch::Approx (1.1111f).margin (0.0001f));
            CHECK (range.convertFrom0to1 (0.1f) == Catch::Approx (6.5435f).margin (0.0001f));
            CHECK (range.convertFrom0to1 (0.01f) == Catch::Approx (9.58617f).margin (0.0001f));
        }

        SECTION ("converts a reversed logarithmic range to linear")
        {
            CHECK (range.convertTo0to1 (0.001f) == Catch::Approx (0.99849f));
            CHECK (range.convertTo0to1 (0.01f) == Catch::Approx (0.98531));
            CHECK (range.convertTo0to1 (1.0f) == Catch::Approx (0.52202f));
            CHECK (range.convertTo0to1 (2.0f) == Catch::Approx (0.37241f));
            CHECK (range.convertTo0to1 (5.0f) == Catch::Approx (0.16294f));
            CHECK (range.convertTo0to1 (9.0f) == Catch::Approx (0.02492f).margin (0.0001f));
            CHECK (range.convertTo0to1 (9.9f) == Catch::Approx (0.00238f).margin (0.0001f));
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

    SECTION ("logarithmicRange 0 to 44100 with exponent of 10", "[parameters]")
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
            // REQUIRE(range.convertTo0to1(441.0f) == 0.1f);
            REQUIRE (range.convertFrom0to1 (0.1f) == Catch::Approx (43.10851f));
            REQUIRE (range.convertFrom0to1 (0.5f) == Catch::Approx (1336.3636f));

            REQUIRE (range.convertTo0to1 (441.f) == Catch::Approx (0.34893f));
            REQUIRE (range.convertTo0to1 (4410.f) == Catch::Approx (0.66907f));
        }

        SECTION ("decibelRangeForHarmonic", "[parameters]")
        {
            SECTION ("unnormalized db values range from -100 to 0")
            {
                auto range = decibelRangeForHarmonic (1);
                REQUIRE (range.convertFrom0to1 (0.0f) == -100.f);
                REQUIRE (range.convertFrom0to1 (1.0f) == 0.0f);

                REQUIRE (range.convertTo0to1 (-100.f) == 0.0f);
                REQUIRE (range.convertTo0to1 (0.0f) == 1.0f);
            }

            SECTION ("handles out of bounds input")
            {
                auto boundsRange = decibelRangeForHarmonic (1);
                REQUIRE (boundsRange.convertTo0to1 (-120.f) == 0.0f);
            }

            //  See docs/Per harmonic db ranges.numbers for example values
            SECTION ("for a harmonic")
            {
                auto range2 = decibelRangeForHarmonic (2);
                auto range3 = decibelRangeForHarmonic (3);
                auto range10 = decibelRangeForHarmonic (10);

                SECTION ("caps the max gain at 1/f, where f is the harmonic number")
                {
                    REQUIRE (range2.convertFrom0to1 (1.0f) == Catch::Approx (juce::Decibels::gainToDecibels (0.5f)));
                    REQUIRE (range3.convertFrom0to1 (1.0f) == Catch::Approx (juce::Decibels::gainToDecibels (0.3333333f)));
                    REQUIRE (range10.convertFrom0to1 (1.0f) == Catch::Approx (juce::Decibels::gainToDecibels (0.1f)));
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
    }

    SECTION ("logarithmic range with linear start 0 to 1")
    {
        auto range = logarithmicRangeWithLinearStart (0.0f, 1.0f, 6.0f, 0.25f, 0.25f);

        SECTION ("handles 0 perfectly")
        {
            REQUIRE (range.convertTo0to1 (0.0f) == 0.0f);
            REQUIRE (range.convertFrom0to1 (0.0f) == 0.0f);
        }

        SECTION ("stays linear for first quarter of the range")
        {
            REQUIRE (range.convertTo0to1 (0.1f) == 0.1f);
            REQUIRE (range.convertFrom0to1 (0.1f) == 0.1f);

            REQUIRE (range.convertTo0to1 (0.2f) == 0.2f);
            REQUIRE (range.convertFrom0to1 (0.2f) == 0.2f);
        }

        SECTION ("goes exponential afterwards")
        {
            REQUIRE (range.convertTo0to1 (0.5f) > 0.6f);
            REQUIRE (range.convertFrom0to1 (0.6f) < 0.4f);
        }

        SECTION ("behaves around the inflection point")
        {
            // should be a bit smaller
            CHECK (range.convertTo0to1 (0.2501f) == Catch::Approx (0.251508296f));
            CHECK (range.convertFrom0to1 (0.2501f) == Catch::Approx (0.250004947f));
        }

        SECTION ("handles 1 perfectly")
        {
            REQUIRE (range.convertTo0to1 (1.0f) == 1.0f);
            REQUIRE (range.convertFrom0to1 (1.0f) == 1.0f);
        }

        SECTION ("logarithmic range with linear to 1000 then exp to 10000")
        {
            range = logarithmicRangeWithLinearStart (0.0f, 10000.0f, 6.0f, 1000);

            SECTION ("handles 0 perfectly")
            {
                REQUIRE (range.convertTo0to1 (0.0f) == 0.0f);
                REQUIRE (range.convertFrom0to1 (0.0f) == 0.0f);
            }

            SECTION ("stays linear for first quarter of the range")
            {
                CHECK (range.convertTo0to1 (500.f) == Catch::Approx (0.125f).margin (0.000001f));
                CHECK (range.convertFrom0to1 (0.125f) == Catch::Approx (500.f).margin (0.000001f));

                CHECK (range.convertTo0to1 (750.f) == Catch::Approx (0.1875f).margin (0.000001f));
                CHECK (range.convertFrom0to1 (0.1875f) == Catch::Approx (750.f).margin (0.000001f));
            }

            SECTION ("goes exponential afterwards")
            {
                CHECK (range.convertTo0to1 (5000.f) > 0.6f);
                CHECK (range.convertFrom0to1 (0.6f) < 4000.f);
            }

            SECTION ("behaves itself around the inflection point")
            {
                CHECK (range.convertTo0to1 (1000.f) == Catch::Approx (0.25f));
                CHECK (range.convertFrom0to1 (0.25f) == Catch::Approx (1000.f));

                CHECK (range.convertTo0to1 (1000.1f) == Catch::Approx (0.250126183f));
                CHECK (range.convertFrom0to1 (0.250126183f) == Catch::Approx (1000.1f));
            }

            SECTION ("handles 1 perfectly")
            {
                REQUIRE (range.convertTo0to1 (10000.f) == Catch::Approx (1.0f));
                REQUIRE (range.convertFrom0to1 (1.0f) == 10000.f);
            }
        }
    }

    SECTION ("decibelRange -30 to 0")
    {
        auto range = decibelRange (-30.0f, 0.0f);

        SECTION ("Boundaries")
        {
            REQUIRE (range.convertFrom0to1 (0.0f) == Catch::Approx (-30.0f).margin (0.001f));

            // still close to -30
            REQUIRE (range.convertFrom0to1 (0.0001f) == Catch::Approx (-30.0f).margin (0.01f));

            REQUIRE (range.convertFrom0to1 (1.0f) == Catch::Approx (0.0f).margin (0.001f));

            // still close to 0
            REQUIRE (range.convertFrom0to1 (0.9999f) == Catch::Approx (0.0f).margin (0.01f));
        }

        SECTION ("Round-trip")
        {
            float originalDb = -10.0f;
            float normalized = range.convertTo0to1 (originalDb);
            float backDb = range.convertFrom0to1 (normalized);
            REQUIRE (backDb == Catch::Approx (originalDb).margin (0.001f));
        }
    }

    SECTION ("decibelRange -3 to +3")
    {
        auto range = decibelRange (-3.0f, 3.0f);

        SECTION ("Boundaries")
        {
            REQUIRE (range.convertFrom0to1 (0.0f) == Catch::Approx (-3.0f).margin (0.001f));
            REQUIRE (range.convertFrom0to1 (1.0f) == Catch::Approx (3.0f).margin (0.001f));
        }

        SECTION ("Round-trip")
        {
            float originalDb = 0.0f;
            float normalized = range.convertTo0to1 (originalDb);
            float backDb = range.convertFrom0to1 (normalized);
            REQUIRE (backDb == Catch::Approx (originalDb).margin (0.001f));
        }

        SECTION ("MidpointCheck")
        {
            float midpoint = 0.5f;
            float dbVal = range.convertFrom0to1 (midpoint);
            // Just an example check: confirm it's near the midpoint in amplitude space
            float expectedAmp = std::pow (10.0f, dbVal / 20.0f);
            float minAmp = std::pow (10.0f, -3.0f / 20.0f);
            float maxAmp = std::pow (10.0f, 3.0f / 20.0f);
            float midAmp = minAmp * std::sqrt (maxAmp / minAmp);
            REQUIRE (expectedAmp == Catch::Approx (midAmp).margin (0.001f));
        }
    }
}
