# pragma once
# include "neuropawn/knight.hpp"
# include "transmission/eeg_messenger.hpp"


void writeSerialCommand(serial::CSerialPort &port, std::string command);

class KnightBoardSerialInterface
{
    private:
    serial::CSerialPort mPort;
    KnightProtocolParser mParser;
    EEGMessenger *mMessenger;
    int mGain;

    void (*mOnWaitStarted)();
    void (*mOnWaitCompleted)();

    void awaitSample(int channelIndex);
    void awaitAnySample();

    public:
    KnightBoardSerialInterface(
        void (onWaitStarted)(), void (onWaitCompleted)()
    ):
        mOnWaitStarted(onWaitStarted),
        mOnWaitCompleted(onWaitCompleted)
    {}

    bool openPort(std::string portName);
    void closePort();
    void initialize(
        int gain, bool useIMUProtocol,
        EEGMessenger *messenger
    );
    void activateChannels(std::vector<int> channelIndices);
};