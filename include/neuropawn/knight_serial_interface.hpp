# pragma once
# include "neuropawn/knight.hpp"
# include "transmission/eeg_messenger.hpp"


class KnightBoardSerialInterface
{
    private:
    serial::CSerialPort mPort;
    KnightProtocolParser mParser;
    EEGMessenger *mMessenger;
    int mGain;

    void (*mOnWaitStarted)();
    void (*mOnWaitCompleted)();

    bool awaitChannelValue(int channelIndex, int timeout = 1000);
    bool awaitPortCondition(std::function<bool()> predicate, int timeout);
    std::function<bool()> buildNewSamplePredicate();

    void ensureChannelConfiguration(int channelIndex, std::string command);

    public:
    KnightBoardSerialInterface(
        void (onWaitStarted)(), void (onWaitCompleted)()
    ):
        mOnWaitStarted(onWaitStarted),
        mOnWaitCompleted(onWaitCompleted)
    {}

    inline bool openPort() { return mPort.open(); }
    inline void closePort() { mPort.close(); }

    void initialize(
        std::string portName,
        int gain, bool useIMUProtocol,
        EEGMessenger *messenger
    );
    void swapProtocolFormat(bool useIMUProtocol);

    bool awaitDeviceResponse(int timeout = 8000);
    bool awaitParsedData(int timeout = 500);

    void activateChannels(std::vector<int> channelIndices);
};