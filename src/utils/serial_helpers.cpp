# include "utils/serial_helpers.hpp"
# include "utils/io_helpers.hpp"


void writeSerialCommand(serial::CSerialPort &port, std::string command)
{
    int commandLength = (int)command.length();
    int writtenBytes = port.writeData(command.c_str(), commandLength);

    if (writtenBytes != commandLength)
    {
        PRINTERR("Failed to configure board.");
        port.close();
        exit(EXIT_FAILURE);
    }
}


std::vector<SerialMessage> splitBufferIntoMessages
(
    const void *buffer, unsigned int bufferSize,
    unsigned int messageLength,
    const unsigned char startByte,
    const unsigned char endByte
)
{
    std::vector<SerialMessage> messages;
    unsigned int offset = 0;
    unsigned int processedSize = 0;
    const unsigned char *ptr = static_cast<const unsigned char*>(buffer);

    bool startByteFound = false;
    unsigned int startByteOffset = 0;

    while (offset < bufferSize)
    {
        if (!startByteFound)
        {
            if (ptr[offset] == startByte) {
                startByteOffset = offset;
                startByteFound = true;
            }
            ++offset;
        }
        else
        {
            const unsigned char* messageStart = ptr + startByteOffset;
            if (messageStart[messageLength - 1] == endByte)
            {
                messages.emplace_back(messageStart, messageLength);
                offset = startByteOffset + messageLength;
                processedSize = offset;
                startByteFound = false;
            }
            else offset = startByteOffset + 1;
            startByteFound = false;
        }
    }

    return messages;
}


long long getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
};