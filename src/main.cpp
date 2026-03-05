# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "interaction/ellipsis_display.hpp"
# include "neuropawn/knight_serial_interface.hpp"
# include "transmission/eeg_messenger.hpp"
# include "utils/io_helpers.hpp"


void HandleInterrupt(int);
static bool interrupted = false;

KnightBoardSerialInterface *boardInterfaceHandle;

EllipsisDisplay ellipsisDisplay;
void startWaitDisplay() { ellipsisDisplay.start(); }
void pauseWaitDisplay() { ellipsisDisplay.pauseAndClearLine(); }

void ensureSuccess(bool result, std::string exitMessage);


int main(int argc, char* argv[])
{
    CommandLineArguments arguments = ParseCommandLineArguments(argc, argv);
    std::string portName = selectPort(arguments.serialPort);

    PRINT("---");
    OUTF("Initializing Board Interface");
    startWaitDisplay();

    KnightBoardSerialInterface boardInterface(
        startWaitDisplay, pauseWaitDisplay
    );

    EEGMessenger messenger {
        arguments.streamName,
        arguments.channelLabels
    };

    boardInterfaceHandle = &boardInterface;
    boardInterface.initialize(
        portName,
        arguments.gain,
        arguments.useIMUProtocol,
        &messenger
    );
    pauseWaitDisplay();

    OUTF("Opening serial port {}", portName);
    startWaitDisplay();
    ensureSuccess(
        boardInterface.openPort(),
        std::format("Failed to open serial port {}.", portName)
    );
    pauseWaitDisplay();

    PRINTF("Opened serial port {}.", portName);
    signal(SIGINT, HandleInterrupt);
    signal(SIGTERM, HandleInterrupt);
    
    COUT("Waiting for device response");
    ensureSuccess(
        boardInterface.awaitDeviceResponse(),
        "Device is not responding."
    );

    COUT("Waiting for data stream");
    if (!boardInterface.awaitParsedData())
    {
        OUTF(
            "Failed to parse {} data, switching format",
            arguments.useIMUProtocol? "IMU": "standard"
        );
        boardInterface.swapProtocolFormat(!arguments.useIMUProtocol);
        ensureSuccess(
            boardInterface.awaitParsedData(),
            "Data does not fit any expected format. Try another device."
        );
        PRINTF(
            "Data fits {} format, consider setting the protocol flag aproproately",
            arguments.useIMUProtocol? "standard": "IMU"
        );
    }

    boardInterface.activateChannels(messenger.getEnabledChannels());
    PRINT("Channel Activation Complete.");
    PRINT("---");

    COUT("Streaming Data");
    startWaitDisplay();

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
        PRINT(std::endl << "-!-");
        PRINT("Closing port.")
        boardInterfaceHandle->closePort();
        exit(EXIT_SUCCESS);
    }
    signal(s, HandleInterrupt);
}

void ensureSuccess(bool result, std::string exitMessage)
{
    if (result) return;
    ellipsisDisplay.stop();
    clear_line();
    PRINTERR(exitMessage);
    boardInterfaceHandle->closePort();
    exit(EXIT_SUCCESS);
}