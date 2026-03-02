# pragma once
# include "neuropawn/knight.hpp"


class EEGMessenger : public IKnightSampleListener
{
    lsl::stream_outlet* mOutlet;
    bool mSampleReceived = false;

    public:
    ~EEGMessenger();
    EEGMessenger(
        std::string streamName,
        std::string montageString,
        std::string sourceId = "Knight_Streamer"
    );
    
    void onSampleReceived(KnightSample sample);
    void awaitSample();
};