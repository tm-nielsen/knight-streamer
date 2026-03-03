# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "interaction/ellipsis_display.hpp"
# include "neuropawn/knight.hpp"
# include "neuropawn/knight_serial_commands.hpp"
# include "transmission/eeg_messenger.hpp"
# include "utils/io_helpers.hpp"


void HandleInterrupt(int);
EllipsisDisplay ellipsisDisplay;
serial::CSerialPort *portHandle;
static bool interrupted = false;


int main(int argc, char* argv[])
{
    CommandLineArguments arguments = ParseCommandLineArguments(argc, argv);
    std::string portName = selectPort(arguments.serialPort);

    PRINT("---");
    OUTF("Opening serial port {}", portName);
    ellipsisDisplay.start();

    serial::CSerialPort port;
    port.init(portName.c_str(), BAUD_RATE);
    bool portOpened = port.open();

    if (!portOpened)
    {
        ellipsisDisplay.stop();
        clear_line();
        PRINTERR("Failed to open serial port " << portName);
        exit(1);
    }
    
    portHandle = &port;
    signal(SIGINT, HandleInterrupt);
    signal(SIGTERM, HandleInterrupt);


    KnightProtocolParser parser = arguments.useIMUProtocol
        ? KnightIMUProtocolParser(arguments.gain)
        : KnightProtocolParser(arguments.gain);
    EEGMessenger messenger {arguments.streamName, arguments.channelLabels};

    parser.setListener(&messenger);
    port.setProtocolParser(&parser);

    sleep(500);
    messenger.awaitSample(0, true);
    ellipsisDisplay.pause();

    PRINTF("Opened serial port {}.", portName);

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        if (
            !arguments.channelLabels.empty() &&
            arguments.channelLabels[i].empty()
        ) continue;

        OUTF("Activating Channel {}", i + 1);
        ellipsisDisplay.start();
        enableKnightBoardEEGChannel(port, i);
        messenger.awaitSample(i);
        ellipsisDisplay.pause();

        OUTF("Adding channel {} to Right Leg Drive", i + 1);
        ellipsisDisplay.start();
        addKnightBoardChannelToRightLegDrive(port, i);
        messenger.awaitSample(i);
        ellipsisDisplay.pause();
    }
    COUT("Streaming Data");
    ellipsisDisplay.start();

    while (true) {
        sleep(100);
    }
}


void HandleInterrupt(int s)
{
    if (!interrupted)
    {
        interrupted = true;
        ellipsisDisplay.stop();
        PRINT("-!-");
        PRINT("Closing port...")
        portHandle->close();
        exit(0);
    }
    signal(s, HandleInterrupt);
}