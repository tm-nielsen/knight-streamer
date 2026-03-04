# include "transmission/eeg_messenger.hpp"


EEGMessenger::EEGMessenger
(
    std::string streamName, 
    std::vector<std::string> channelLabels,
    std::string sourceId
)
{
    bool channelsLabelled = !channelLabels.empty();
    int enabledChannelCount = 0;
    enabledChannels = {};

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        if (channelsLabelled && channelLabels[i].empty()) continue;
        enabledChannels.emplace_back(i);
        enabledChannelCount++;
    }

    lsl::stream_info info (
        streamName, "EEG", enabledChannelCount,
        SAMPLE_RATE, lsl::cf_float32, sourceId
    );

    lsl::xml_element description = info.desc();
    description.append_child_value("manufacturer", "NeuroPawn");

    lsl::xml_element channels = description.append_child("channels");
    for (int channelIndex : enabledChannels)
    {
        lsl::xml_element ch = channels.append_child("channel");
        ch.append_child_value("unit", "microvolts");
        ch.append_child_value("type", "EEG");

        if (channelsLabelled)
        {
            ch.append_child_value("label", channelLabels[channelIndex]);
        }
    }

    mOutlet = new lsl::stream_outlet(info);
}

EEGMessenger::~EEGMessenger()
{
    delete mOutlet;
}


void EEGMessenger::onSampleReceived(KnightSample sample)
{
    mLatestSample = sample;

    std::vector<double> activeChannelValues {};
    for (int channelIndex : enabledChannels)
    {
        activeChannelValues.emplace_back(sample[channelIndex]);
    }
    mOutlet->push_sample(activeChannelValues);
}