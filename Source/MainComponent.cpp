#include "MainComponent.h"

MainComponent::MainComponent()
{
  
    audioDeviceManager.initialiseWithDefaultDevices(1, 0); // 1 input (mic), 0 outputs
    audioDeviceManager.addAudioCallback(this);

    addAndMakeVisible(sensitivitySlider);
    sensitivitySlider.setRange(0.1f, 2.0f, 0.01f);
    sensitivitySlider.setValue(1.0f);
    sensitivitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sensitivitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    sensitivitySlider.onValueChange = [this] { sensitivity = sensitivitySlider.getValue(); };

    addAndMakeVisible(sensitivityLabel);
    sensitivityLabel.setText("Sensitivity", juce::dontSendNotification);
    sensitivityLabel.setJustificationType(juce::Justification::centred);

    setSize(600, 400); // Default size, will adapt on Android
    startTimerHz(60);  // 60 FPS repaint
}

MainComponent::~MainComponent()
{
    audioDeviceManager.removeAudioCallback(this);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    // Gradient background
    float maxAmplitude = *std::max_element(audioBuffer.begin(), audioBuffer.end());
    juce::ColourGradient gradient(juce::Colours::purple.interpolatedWith(juce::Colours::cyan, maxAmplitude * sensitivity),
        0, 0, juce::Colours::blue, getWidth(), getHeight(), false);
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    // Waveform
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    juce::Path waveform;
    waveform.startNewSubPath(0, getHeight() / 2);
    for (int x = 0; x < getWidth(); ++x)
    {
        float sampleIndex = (x / (float)getWidth()) * bufferSize;
        float sample = audioBuffer[static_cast<int>(sampleIndex)];
        float y = getHeight() / 2 + sample * (getHeight() / 2.5f) * sensitivity;
        waveform.lineTo(x, y);
    }
    g.strokePath(waveform, juce::PathStrokeType(2.0f));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    sensitivityLabel.setBounds(bounds.removeFromBottom(60).removeFromTop(20).reduced(10, 0));
    sensitivitySlider.setBounds(bounds.removeFromBottom(40).reduced(50, 0));
}

void MainComponent::timerCallback()
{
    repaint();
}

void MainComponent::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
    int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext&)
{
    if (numInputChannels > 0)
    {
        const float* inputData = inputChannelData[0];
        for (int i = 0; i < numSamples && i < bufferSize; ++i)
        {
            audioBuffer[i] = inputData[i];
        }
    }
}