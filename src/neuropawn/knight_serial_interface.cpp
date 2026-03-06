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
    mGain = gain;

    mPort.init(portName.c_str(), BAUD_RATE);
    mPort.setOperateMode(serial::NativeSynchronousOperate);

    mMessenger = messenger;
    mParser = KnightProtocolParser(
        gain, useIMUProtocol
        ? KnightProtocolParser::IMU_FORMAT
        : KnightProtocolParser::STANDARD_FORMAT
    );

    mParser.setListener(mMessenger);
}

void KnightBoardSerialInterface::swapProtocolFormat(bool useIMUProtocol)
{
    mParser.applyFormat(
        useIMUProtocol
        ? KnightProtocolParser::IMU_FORMAT
        : KnightProtocolParser::STANDARD_FORMAT
    );
}


bool KnightBoardSerialInterface::awaitSerialData(int timeout)
{
    discardReadBuffer();
    return awaitCondition(
        [this](){ return mParser.hasReceivedData(); }
        , timeout
    );
}

bool KnightBoardSerialInterface::awaitParsedData(int timeout)
{
    return awaitCondition(
        [this](){ return mParser.hasParsedMessage(); }
        , timeout
    );
}


void KnightBoardSerialInterface::activateChannels(std::vector<int> channelIndices)
{    
    for (int channelIndex : channelIndices)
    {
        int pinIndex = channelIndex + 1;

        ensureChannelConfiguration(
            channelIndex, std::format("chon_{}_{}", pinIndex, mGain),
            std::format("Activating Channel {}", pinIndex)
        );
        ensureChannelConfiguration(
            channelIndex, std::format("rldadd_{}", pinIndex),
            std::format("Adding channel {} to Right Leg Drive", pinIndex)
        );
    }
}


void KnightBoardSerialInterface::ensureChannelConfiguration
(
    int channelIndex, std::string command,
    std::string commandDescription
)
{
    bool channelValueReceived = false;
    while (!channelValueReceived)
    {
        COUT(commandDescription);
        writeSerialCommand(mPort, command);
        if (awaitChannelValue(channelIndex)) return;
        else
        {
            PRINT("No response from board, trying again.");
        }
    }
}


bool KnightBoardSerialInterface::awaitChannelValue(int channelIndex, int timeout)
{
    discardReadBuffer();
    mMessenger->resetChannelReceptionFlag(channelIndex);
    return awaitCondition(
        [channelIndex, this]() { return mMessenger-> hasReceivedChannelValue(channelIndex); }
        , timeout
    );
}

bool KnightBoardSerialInterface::awaitCondition(std::function<bool()> predicate, int timeout)
{
    if (mOnWaitStarted) mOnWaitStarted();

    long long startTime = getCurrentTimestamp();
    while (!predicate())
    {
        readAndParseUsedBuffer();

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


void KnightBoardSerialInterface::streamData()
{
    while(readAndParseUsedBuffer() >= 0) sleep(1);
}

unsigned int KnightBoardSerialInterface::readAndParseUsedBuffer()
{
    char buffer[4096];
    unsigned int usedBufferLength = mPort.getReadBufferUsedLen();
    if (usedBufferLength < 2 * mParser.messageLength()) return 0;

    mPort.readData(buffer, usedBufferLength);
    return mParser.processBuffer(buffer, usedBufferLength);
}

void KnightBoardSerialInterface::discardReadBuffer()
{
    char dumpster[4096];
    mPort.readAllData(dumpster);
}