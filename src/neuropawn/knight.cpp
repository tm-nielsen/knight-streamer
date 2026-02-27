# include "neuropawn/knight.hpp"
# include "utils/cast.h"
# include "utils/print_helpers.hpp"


unsigned int KnightProtocolParser::parse
(
    const void *buffer, unsigned int size,
    ResultVector &results
)
{
    const unsigned char LENGTH = messageLength();
    unsigned int offset = 0;
    unsigned int processedSize = 0;
    const unsigned char *ptr = static_cast<const unsigned char*>(buffer);

    bool startByteFound = false;
    unsigned int startByteOffset = 0;

    while (offset < size)
    {
        if (!startByteFound)
        {
            if (ptr[offset] == START_BYTE) {
                startByteOffset = offset;
                startByteFound = true;
            }
            ++offset;
        }
        else
        {
            const unsigned char* message = ptr + startByteOffset;
            if (message[LENGTH - 1] == END_BYTE)
            {
                results.emplace_back(message, LENGTH);
                offset = startByteOffset + LENGTH;
                processedSize = offset;
                startByteFound = false;
            }
            else offset = startByteOffset + 1;
            startByteFound = false;
        }
    }

    return processedSize;
}

void KnightProtocolParser::onProtocolEvent(ResultVector &results)
{
    for (const serial::IProtocolResult& result : results)
    {
        KnightSample sample = parseSample(result);
        PRINTF("Sample {} received, channel 1 value : {}", sample.counter, sample[0]);
    }
}

KnightSample KnightProtocolParser::parseSample(serial::IProtocolResult result)
{
    KnightSample sample;
    sample.counter = result.data[1];

    const unsigned char* exgData = result.data + EXG_DATA_OFFSET;
    double eegScale = mGain * BASE_EEG_SCALE;

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        const unsigned char* channelData = exgData + 2 * i;
        unsigned int channelValue = castTwoByteInteger(channelData);
        sample.eegChannels[i] = channelValue * eegScale;
    }

    sample.lOffStatP = result.data[LOffStatP_DATA_OFFSET];
    sample.lOffStatN = result.data[LOffStatN_DATA_OFFSET];
    return sample;
}