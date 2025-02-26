#pragma once

#include <JuceHeader.h>  

//==============================================================================
class MainComponent : public juce::Component,
    private juce::Timer,
    private juce::AudioIODeviceCallback
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice*) override {}
    void audioDeviceStopped() override {}

    juce::AudioDeviceManager audioDeviceManager;
    static constexpr int bufferSize = 1024;
    std::array<float, bufferSize> audioBuffer = { 0 };
    float sensitivity = 1.0f;

    juce::Slider sensitivitySlider;
    juce::Label sensitivityLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};