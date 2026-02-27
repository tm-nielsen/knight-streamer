# include "neuropawn/knight.hpp"
# include "utils/cast.h"


KnightIMUSample::KnightIMUSample(KnightSample source)
{
    counter = source.counter;
    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        eegChannels[i] = source.eegChannels[i];
    }
    lOffStatP = source.lOffStatP;
    lOffStatN = source.lOffStatN;
}

KnightSample KnightIMUProtocolParser::parseSample(serial::IProtocolResult result)
{
    KnightIMUSample sample = KnightProtocolParser::parseSample(result);

    const unsigned char* imuData = result.data + IMU_DATA_OFFSET;

    sample.acceleration = castVector3(imuData);
    sample.gyro = castVector3(imuData + 12);
    sample.mag = castVector3(imuData + 24);
    return sample;
}