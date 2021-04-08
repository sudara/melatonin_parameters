# Melatonin Parameters

This is a very basic C++ JUCE module that contains

1. A logarithmic `NormalizableRange` "factory" that accepts an exponent value to increase the amount of skew.
2. `stringFromTimeValue` and `timeValueFromString` that behave in a user-friendly way.
3. Tests for the above written in [Catch2](https://github.com/catchorg/Catch2).

These are the parameter behaviors I want by default a lot of the time. Something like this could also make sense in JUCE proper.

When in doubt see the tests for behavior specifics.

## How to use

This is a juce module. 

If you are a solo coder using Projucer, you could git clone and then manually add the module your project via the UI.

I'm not a fan of copying and pasting dependencies (IMO that's one of reasons we can't have nice things in C++). This will set you up with a git submodule tracking the `main` branch:
```
git submodule add -b main https://github.com/sudara/melatonin_parameters
git commit -m "Added melatonin_parameters submodule."
```

Inform JUCE about the module in your `CMakeLists.txt`:
```
juce_add_module("modules/melatonin_parameters")
```

When you want to update melatonin_parameters, you can run
```
git submodule update --remote --merge melatonin_parameter
```

zzzzzz.... Wake me up when C++ has widely supported package management plzthxbai.


## Running tests

Catch2 tests are in `melatonin_parameters.cpp` surrounded by `if RUN_MELATONIN_TESTS`

Assuming you use CMake:

1. Have Catch2 setup in your project. See [pamplejuce](https://github.com/sudara/pamplejuce).
   
2. Add the module as above
2. Add the preprocessor flag to the test binary with: 
   
`target_compile_definitions(Tests PRIVATE RUN_MELATONIN_TESTS=1)` 

where `Tests` is the name of your test binary.

## Logarithmic Range


### How to use


With the default exponent setting of 6:

```cpp
juce::AudioParameterFloat ("release", "Release", logarithmicRange (0, 15.0f), 0.1f),
```

With a custom exponent (10 might be a sensible value for frequency):

```cpp
juce::AudioParameterFloat ("frequency", "Frequency", logarithmicRange (20.0f, 20000.0f, 10.0f), 0.1f),
```


### Why

[Auditory perception of time and frequency is logarithmic](https://en.wikipedia.org/wiki/Weber–Fechner_law), so it's a good default for many knobs in audio.

Imagine a release knob. The first little bit of the knob, you'd probably want some fine control over 0-100ms to craft short release times. Maybe by the time you get to the middle of the knob it's at 1s. And then the rest of the knob can handle 1-10s, where granularity is much less important.

### Implementation

[![Screenshot 2021-04-08 Linear to logarithmic range conversion - Safari](https://user-images.githubusercontent.com/472/114071474-73822e00-98a1-11eb-95a3-405faae1e768.jpg)](https://www.desmos.com/calculator/qkc6naksy5)

The math can be viewed at [this desmos link](https://www.desmos.com/calculator/qkc6naksy5).

<details>

Github can't render latex, but here's the latex formulas in case the desmos link goes away.

From a normalized 0-1 to an unnormalized y0 to y1:
```
y_{0}\ +\frac{2^{kx}-1}{2^{k}-1}\left(y_{1}-y_{0}\right)

```

To a normalized 0-1 from an unnormalized y0 to y1
```
\frac{\log_{2}\left(\frac{x-y_{0}}{y_{1}-y_{0}}\left(2^{k}-1\right)+1\right)}{k}
```

</details>

Understanding the implementation is a bit of a pain, but the core idea is to be able to translate to and from any logarithmic range to JUCE's normalized 0-1 range.

See the references for more detail. Solutions I ran into had oddities such as not allowing the logarithmic minimum to be 0, as it would result in division by 0. 

I went back to the math to cook up something that I could understand, where the range could start at 0, and where the exponential-ness (skew) and starting positions could be adjusted.


## stringFromTimeValue and timeValueFromString

### How to use

```cpp
juce::AudioParameterFloat> ("delay", "Delay", logarithmicRange(0.0f, 1.0f), 0.0f, juce::String(), juce::AudioProcessorParameter::genericParameter, stringFromTimeValue, timeValueFromString)
```

### Why?
In JUCE, parameters don't "know" what units they are nor how to display them. Even though most plugins will have similar values such as frequency and time and "note value", the formatting display of those values is up to the user; there are no out of the box defaults.

## stringFromTimeValue Implementation

This is is optimized for normal person usability, *not* for accuracy. 

1. Under 0.5s, values are displayed as ms with NO decimal places. It will look like `1ms`, `5ms`, etc.
2. Excess 0s are removed. `1.00` is displayed as `1s`
3. A max of two digits after the decimal place. `1.111` will display as `1.11`.



## timeValueFromString Implementation

1. Values can be with or without decimal

1. `ms` and `s` are accepted as units: `0ms`, `11.1ms`, `100ms`, `1.0s`, `15.98s` are all valid.

3. Without a unit, single digits or a decimal place will trigger seconds conversion: `1` or `1.` or `1.0` 
   
4. Without a unit, double and triple digits convert to ms. 
   
See the tests for more detail.

## References

* https://github.com/ffAudio/foleys_gui_magic/blob/master/Helpers/foleys_Conversions.h#L41
* https://forum.juce.com/t/logarithmic-slider-for-frequencies-iir-hpf/37569/21
* https://forum.cockos.com/showpost.php?p=2017848&postcount=11

## TODO

* Implement and test snapToLegalValue
* Implement versioning to make it safer to use via CMake
