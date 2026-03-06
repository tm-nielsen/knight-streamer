# pragma once
# include "knight_protocol_constants.h"
# include "utils/serial_helpers.hpp"
# include "utils/vector3.hpp"

using namespace KnightProtocolConstants;


struct KnightSample
{
    uint8_t counter;
    double eegChannels[CHANNEL_COUNT];
    double lOffStatP;
    double lOffStatN;

    KnightSample() {}
    KnightSample(const unsigned char* data, int gain);
    static KnightSample parse(const unsigned char* data, int gain);
    double& operator[](int i) { return eegChannels[i]; }
};

struct KnightIMUSample : KnightSample
{
    Vector3 acceleration;
    Vector3 mag;
    Vector3 gyro;

    KnightIMUSample(const unsigned char* data, int gain);
    static KnightSample parse(const unsigned char* data, int gain);
};


class IKnightSampleListener
{
    public:
    virtual void onSampleReceived(KnightSample) {}
};


class KnightProtocolParser
{
    protected:
    KnightSample (*mSampleConstructor)(const unsigned char* data, int gain);
    IKnightSampleListener* mListener;

    int mGain;
    unsigned char mMessageLength;
    bool mHasReceivedData;
    bool mHasParsedMessage;

    void notifyListener(std::vector<SerialMessage> messages);

    public:
    enum ProtocolFormat { STANDARD, IMU };
    static const ProtocolFormat STANDARD_FORMAT = ProtocolFormat::STANDARD;
    static const ProtocolFormat IMU_FORMAT = ProtocolFormat::IMU;

    KnightProtocolParser(int gain = 12, ProtocolFormat format = STANDARD_FORMAT)
    : mGain(gain){ applyFormat(format); }

    void applyFormat(ProtocolFormat format);
    unsigned int processBuffer(const void *buffer, unsigned int size);

    void setListener(IKnightSampleListener* listener);
    void removeListener();

    unsigned int messageLength() { return mMessageLength; }
    bool hasReceivedData() { return mHasReceivedData; }
    bool hasParsedMessage() { return mHasParsedMessage; }
};