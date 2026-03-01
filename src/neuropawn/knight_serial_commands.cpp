# include "neuropawn/knight_serial_commands.hpp"
# include "utils/print_helpers.hpp"


bool enableKnightBoardEEGChannel(serial::CSerialPort &port, int channelIndex)
{
    writeSerialCommand(port, std::format("chon_{}_12", channelIndex));
    writeSerialCommand(port, std::format("rldadd_{}", channelIndex));
    return true;
}

bool writeSerialCommand(serial::CSerialPort &port, std::string command)
{
    int commandLength = (int)command.length();
    int writtenBytes = port.writeData(command.c_str(), commandLength);
    sleep(1);
    if (writtenBytes == commandLength) return true;

    PRINTERR("Failed to configure board.");
    port.close();
    exit(1);
}