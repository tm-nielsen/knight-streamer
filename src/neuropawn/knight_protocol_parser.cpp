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

unsigned int KnightProtocolParser::parse
(
    const void *buffer, unsigned int size,
    ResultVector &results
)
{
    mHasReceivedData = true;
    return splitBufferIntoMessages(
        buffer, size,
        mMessageLength,
        START_BYTE, END_BYTE,
        results
    );
}


void KnightProtocolParser::onProtocolEvent(ResultVector &results)
{
    for (const serial::IProtocolResult& result : results)
    {
        KnightSample sample = mSampleConstructor(result.data, mGain);
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