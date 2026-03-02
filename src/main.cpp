# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "neuropawn/knight.hpp"
# include "neuropawn/knight_serial_commands.hpp"
# include "transmission/eeg_messenger.hpp"
# include "utils/print_helpers.hpp"


void HandleInterrupt(int);
static volatile bool interrupted = false;


int main(int argc, char* argv[])
{
    CommandLineArguments arguments = ParseCommandLineArguments(argc, argv);

    if (arguments.montageString.empty())
    {
        PRINT("Montage not specified, stream will be missing channel label metadata.");
    }
    std::string portName = selectPort(arguments.serialPort);

    serial::CSerialPort port;
    port.init(portName.c_str(), BAUD_RATE);
    
    bool portOpened = port.open();
    if (!portOpened)
    {
        PRINTERR("Failed to open serial port " << portName);
        exit(1);
    }
    
    signal(SIGINT, HandleInterrupt);
    signal(SIGTERM, HandleInterrupt);


    KnightProtocolParser parser = arguments.useIMUProtocol
        ? KnightIMUProtocolParser(arguments.gain)
        : KnightProtocolParser(arguments.gain);
    EEGMessenger messenger {arguments.streamName, arguments.montageString};

    parser.setListener(&messenger);
    port.setProtocolParser(&parser);

    PRINT("---");
    PRINTF("Opened serial port {}.", portName)
    sleep(2000);

    for (int i = 1; i < CHANNEL_COUNT + 1; i++)
    {
        enableKnightBoardEEGChannel(port, 1);
    }
    PRINT("Activated channels")

    while (!interrupted) { sleep(100); }

    PRINT("Closing port...")
    port.close();
}


void HandleInterrupt(int s)
{
    if (!interrupted)
    {
        interrupted = true;
        PRINT("---");
    }
    signal(s, HandleInterrupt);
}