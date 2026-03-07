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
    mPort.setReadBufferSize(BUFFER_LENGTH);

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
    mPort.flushReadBuffers();
    return awaitCondition(
        [this](){ return mParser.hasReceivedData(); }
        , timeout
    );
}

bool KnightBoardSerialInterface::awaitParsedData(int timeout)
{
    return alignData() && awaitCondition(
        [this](){ return mParser.hasParsedMessage(); }
        , timeout
    );
}

bool KnightBoardSerialInterface::alignData(int timeout)
{
    char readByte[1];
    unsigned char readCount = 0;

    while (readByte[0] != START_BYTE && readCount < MAXIMUM_MESSAGE_LENGTH)
    {
        int readBytes = mPort.readData(readByte, 1);
        if (readBytes > 0) readCount += (unsigned char)readBytes;
        if (readCount > MAXIMUM_MESSAGE_LENGTH) return false;
        sleep(1);
    }

    long long startTime = getCurrentTimestamp();
    unsigned int remainingMessageLength = mParser.messageLength() - 1;
    while (mPort.getReadBufferUsedLen() < remainingMessageLength)
    {
        sleep(10);
        if (getCurrentTimestamp() - startTime > timeout) return false;
    }

    char buffer[BUFFER_LENGTH];
    return mPort.readData(buffer, remainingMessageLength) == (int)remainingMessageLength;
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
        mPort.flushWriteBuffers();
        sleep(10);
        if (awaitChannelValue(channelIndex)) return;
        else
        {
            PRINT("No response from board, trying again.");
        }
    }
}


bool KnightBoardSerialInterface::awaitChannelValue(int channelIndex, int timeout)
{
    mPort.flushReadBuffers();
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
    char buffer[BUFFER_LENGTH];
    unsigned int usedBufferLength = mPort.getReadBufferUsedLen();
    unsigned int messageLength = mParser.messageLength();

    unsigned int messageCount = usedBufferLength / messageLength;
    if (messageCount == 0) return 0;

    unsigned int useableBufferLength = messageCount * messageLength;

    mPort.readData(buffer, useableBufferLength);
    return mParser.processBuffer(buffer, useableBufferLength);
}