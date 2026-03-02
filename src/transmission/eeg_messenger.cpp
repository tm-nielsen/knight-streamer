# include "transmission/eeg_messenger.hpp"


EEGMessenger::EEGMessenger
(
    std::string streamName, 
    std::string montageString,
    std::string sourceId
)
{
    lsl::stream_info info (
        streamName, "EEG", CHANNEL_COUNT,
        SAMPLE_RATE, lsl::cf_float32, sourceId
    );

    lsl::xml_element description = info.desc();
    description.append_child_value("manufacturer", "NeuroPawn");

    std::vector<std::string> channelLabels = splitString(montageString);

    lsl::xml_element channels = description.append_child("channels");
    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        lsl::xml_element ch = channels.append_child("channel");
        ch.append_child_value("label", channelLabels[i]);
        ch.append_child_value("unit", "microvolts");
        ch.append_child_value("type", "EEG");
    }

    mOutlet = new lsl::stream_outlet(info);
}

EEGMessenger::~EEGMessenger()
{
    delete mOutlet;
}


void EEGMessenger::onSampleReceived(KnightSample sample)
{
    mOutlet->push_sample(sample.eegChannels);
}