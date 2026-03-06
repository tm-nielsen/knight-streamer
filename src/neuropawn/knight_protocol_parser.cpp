# include "neuropawn/knight.hpp"
# include "utils/io_helpers.hpp"


void KnightProtocolParser::applyFormat(ProtocolFormat format)
{
    switch (format)
    {
        case STANDARD_FORMAT:
            mMessageLength = MESSAGE_LENGTH;
            mSampleConstructor = KnightSample::parse;
            break;
        case IMU_FORMAT:
            mMessageLength = IMU_MESSAGE_LENGTH;
            mSampleConstructor = KnightIMUSample::parse;
            break;
    }
}

unsigned int KnightProtocolParser::processBuffer
(
    const void *buffer, unsigned int size
)
{
    std::vector<SerialMessage> messages = splitBufferIntoMessages(
        buffer, size,
        mMessageLength,
        START_BYTE, END_BYTE
    );
    mHasReceivedData |= size > 0;
    mHasParsedMessage |= !messages.empty();

    if (mListener) notifyListener(messages);
    return (unsigned int)messages.size();
}

void KnightProtocolParser::notifyListener(std::vector<SerialMessage> messages)
{
    if (!mListener) return;
    for (const SerialMessage message : messages)
    {
        KnightSample sample = mSampleConstructor(message.data, mGain);
        if (mListener) mListener->onSampleReceived(sample);
    }
}


void KnightProtocolParser::setListener(IKnightSampleListener* listener)
{
    mListener = listener;
}
void KnightProtocolParser::removeListener()
{
    mListener = nullptr;
}