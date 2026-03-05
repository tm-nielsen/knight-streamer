# include "neuropawn/knight_serial_interface.hpp"
# include "utils/io_helpers.hpp"
# include "utils/serial_helpers.hpp"


void KnightBoardSerialInterface::initialize
(
    std::string portName,
    int gain, bool useIMUProtocol,
    EEGMessenger *messenger
)
{
    mPort.init(portName.c_str(), BAUD_RATE);
    mGain = gain;
    mMessenger = messenger;
    mParser = KnightProtocolParser(
        gain, useIMUProtocol
        ? KnightProtocolParser::IMU_FORMAT
        : KnightProtocolParser::STANDARD_FORMAT
    );

    mParser.setListener(mMessenger);
    mPort.setProtocolParser(&mParser);
}

void KnightBoardSerialInterface::swapProtocolFormat(bool useIMUProtocol)
{
    mParser.applyFormat(
        useIMUProtocol
        ? KnightProtocolParser::IMU_FORMAT
        : KnightProtocolParser::STANDARD_FORMAT
    );
}


bool KnightBoardSerialInterface::awaitDeviceResponse(int timeout)
{
    return awaitPortCondition(
        [this](){ return mParser.hasReceivedData(); },
        timeout
    );
}

bool KnightBoardSerialInterface::awaitParsedData(int timeout)
{
    return awaitPortCondition(buildNewSamplePredicate(), timeout);
}


void KnightBoardSerialInterface::activateChannels(std::vector<int> channelIndices)
{    
    for (int channelIndex : channelIndices)
    {
        int pinIndex = channelIndex + 1;

        OUTF("Activating Channel {}", pinIndex);
        ensureChannelConfiguration(
            channelIndex, std::format("chon_{}_{}", pinIndex, mGain)
        );

        OUTF("Adding channel {} to Right Leg Drive", pinIndex);
        ensureChannelConfiguration(
            channelIndex, std::format("rldadd_{}", pinIndex)
        );
    }
}


void KnightBoardSerialInterface::ensureChannelConfiguration(int channelIndex, std::string command)
{
    bool channelValueReceived = false;
    while (!channelValueReceived)
    {
        writeSerialCommand(mPort, command);
        if (!awaitChannelValue(channelIndex))
        {
            PRINT("No response from board, trying again.");
        }
    }
}


bool KnightBoardSerialInterface::awaitChannelValue(int channelIndex, int timeout)
{
    auto newSamplePredicate = buildNewSamplePredicate();
    return awaitPortCondition(
        [newSamplePredicate, channelIndex, this]()
        {
            return newSamplePredicate() &&
            mMessenger->getLatestChannelValue(channelIndex) != 0;
        },
        timeout
    );
}

bool KnightBoardSerialInterface::awaitPortCondition(std::function<bool()> predicate, int timeout)
{
    if (mOnWaitStarted) mOnWaitStarted();
    mPort.flushBuffers();

    long long startTime = getCurrentTimestamp();
    while (!predicate())
    {
        sleep(10);
        long long currentTime = getCurrentTimestamp();
        if (currentTime - startTime > timeout)
        {
            if (mOnWaitCompleted) mOnWaitCompleted();
            return false;
        }
    }

    if (mOnWaitCompleted) mOnWaitCompleted();
    return true;
}

std::function<bool()> KnightBoardSerialInterface::buildNewSamplePredicate()
{
    unsigned char lastCounterValue = mMessenger->getLatestCounter();
    return [lastCounterValue, this]()
    { return mMessenger->getLatestCounter() != lastCounterValue; };
}