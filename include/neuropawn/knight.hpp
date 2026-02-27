# pragma once
# include "knight_protocol_constants.h"
# include "utils/vector3.hpp"

using namespace KnightProtocolConstants;
using ResultVector = std::vector<serial::IProtocolResult>;


struct KnightSample
{
    uint8_t counter;
    double eegChannels[CHANNEL_COUNT];
    double lOffStatP;
    double lOffStatN;

    double& operator[](int i) { return eegChannels[i]; }
};

struct KnightIMUSample : KnightSample
{
    Vector3 acceleration;
    Vector3 mag;
    Vector3 gyro;

    KnightIMUSample(KnightSample sample);
};


class KnightProtocolParser : public serial::IProtocolParser
{
    protected:
    double mGain = 4;

    virtual const unsigned char messageLength()
    { return MESSAGE_LENGTH; }

    public:
    KnightProtocolParser(double gain): mGain(gain) {}

    unsigned int parse(const void *buffer, unsigned int size,ResultVector &results);
    void onProtocolEvent(ResultVector &results);

    virtual KnightSample parseSample(serial::IProtocolResult result); 
};

class KnightIMUProtocolParser : public KnightProtocolParser
{
    const unsigned char messageLength()
    { return IMU_MESSAGE_LENGTH; }

    KnightSample parseSample(serial::IProtocolResult result) override;
};