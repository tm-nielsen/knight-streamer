# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "neuropawn/knight.hpp"
# include "neuropawn/knight_serial_commands.hpp"
# include "transmission/eeg_messenger.hpp"
# include "utils/print_helpers.hpp"


void HandleInterrupt(int);
serial::CSerialPort *portHandle;
static bool interrupted = false;


int main(int argc, char* argv[])
{
    CommandLineArguments arguments = ParseCommandLineArguments(argc, argv);

    if (arguments.montageString.empty())
    {
        PRINT("Montage not specified, stream will be missing channel label metadata.");
    }
    std::string portName = selectPort(arguments.serialPort);

    PRINT("---");
    OUTF("Opening serial port {}...", portName);

    serial::CSerialPort port;
    port.init(portName.c_str(), BAUD_RATE);
    bool portOpened = port.open();

    if (!portOpened)
    {
        PRINTERR("Failed to open serial port " << portName);
        exit(1);
    }
    
    portHandle = &port;
    signal(SIGINT, HandleInterrupt);
    signal(SIGTERM, HandleInterrupt);


    KnightProtocolParser parser = arguments.useIMUProtocol
        ? KnightIMUProtocolParser(arguments.gain)
        : KnightProtocolParser(arguments.gain);
    EEGMessenger messenger {arguments.streamName, arguments.montageString};

    parser.setListener(&messenger);
    port.setProtocolParser(&parser);

    sleep(500);
    messenger.awaitSample(0, true);
    clear_line();
    PRINTF("Opened serial port {}.", portName);

    for (int i = 1; i < CHANNEL_COUNT + 1; i++)
    {
        OUTF("Activating Channel {}...", i);
        enableKnightBoardEEGChannel(port, i);
        messenger.awaitSample(i - 1);
        clear_line();

        OUTF("Adding channel {} to Right Leg Drive...", i);
        addKnightBoardChannelToRightLegDrive(port, i);
        messenger.awaitSample(i - 1);
        clear_line();
    }
    PRINT("Streaming Data.");

    while (true) { sleep(100); }
}


void HandleInterrupt(int s)
{
    if (!interrupted)
    {
        interrupted = true;
        PRINT("-!-");
        PRINT("Closing port...")
        portHandle->close();
        exit(0);
    }
    signal(s, HandleInterrupt);
}