# pragma once

using ResultVector = std::vector<serial::IProtocolResult>;


long long getCurrentTimestamp();

void writeSerialCommand(serial::CSerialPort &port, std::string command);


struct SerialMessage {
    unsigned char data[256];
    unsigned int mLength;

    SerialMessage(const unsigned char* buffer, unsigned int length)
    {
        mLength = length;
        for (unsigned int i = 0; i < length; i++) data[i] = buffer[i];
    }
};

std::vector<SerialMessage> splitBufferIntoMessages(
    const void *buffer, unsigned int bufferSize,
    unsigned int messageLength,
    const unsigned char StartByte,
    const unsigned char EndByte
);