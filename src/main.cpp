# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "neuropawn/knight.hpp"
# include "transmission/eeg_messenger.hpp"
# include "utils/print_helpers.hpp"


int main(int argc, char* argv[])
{
    CommandLineArguments arguments = ParseCommandLineArguments(argc, argv);

    if (arguments.montage.empty())
    {
        PRINT("Montage not specified, stream will be missing channel label metadata.");
    }
    std::string portName = selectPort(arguments.serialPort);

    serial::CSerialPort port;
    port.init(
        portName.c_str(), 115200,
        serial::ParityNone,
        serial::DataBits8,
        serial::StopTwo
    );
    
    bool portOpened = port.open();
    if (!portOpened)
    {
        PRINTERR("Failed to open serial port " << portName);
        exit(1);
    }

    PRINTF("Opened serial port {}.", portName)

    port.setOperateMode(serial::SynchronousOperate);
    for (int i = 1; i < CHANNEL_COUNT + 1; i++)
    {
        port.writeData(std::format("chon_{}_12", i).c_str(), 9);
        port.writeData(std::format("rldadd_{}", i).c_str(), 8);
    }

    PRINT("Activated channels")
    
    KnightProtocolParser parser = arguments.useIMUProtocol
        ? KnightIMUProtocolParser(arguments.gain)
        : KnightProtocolParser(arguments.gain);
    EEGMessenger messenger {arguments.streamName};

    port.setOperateMode(serial::AsynchronousOperate);
    parser.setListener(&messenger);
    port.setProtocolParser(&parser);

    for (;;) { sleep(100); }
}