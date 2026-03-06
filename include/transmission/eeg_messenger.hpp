# pragma once
# include "neuropawn/knight.hpp"


class EEGMessenger : public IKnightSampleListener
{
    lsl::stream_outlet* mOutlet;
    std::vector<int> enabledChannels;

    bool mChannelValueReceptionFlags [CHANNEL_COUNT];

    public:
    ~EEGMessenger();
    EEGMessenger(
        std::string streamName,
        std::vector<std::string> channelLabels,
        std::string sourceId = "Knight_Streamer"
    );

    void onSampleReceived(KnightSample sample);

    std::vector<int> getEnabledChannels() { return enabledChannels; }
    bool hasReceivedChannelValue(int channelIndex) { return mChannelValueReceptionFlags[channelIndex]; }
    void resetChannelReceptionFlag(int channelIndex) { mChannelValueReceptionFlags[channelIndex] = false; }
};