# pragma once
# include "neuropawn/knight.hpp"


class EEGMessenger : public IKnightSampleListener
{
    lsl::stream_outlet* mOutlet;
    KnightSample mLatestSample;
    std::vector<int> enabledChannels;

    public:
    ~EEGMessenger();
    EEGMessenger(
        std::string streamName,
        std::vector<std::string> channelLabels,
        std::string sourceId = "Knight_Streamer"
    );

    std::vector<int> getEnabledChannels() { return enabledChannels; }
    unsigned char getLatestCounter() { return mLatestSample.counter; }
    double getLatestChannelValue(int channelIndex) { return mLatestSample[channelIndex]; }
    
    void onSampleReceived(KnightSample sample);
};