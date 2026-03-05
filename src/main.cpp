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
    
    boardInterfaceHandle = &boardInterface;
    signal(SIGINT, HandleInterrupt);
    signal(SIGTERM, HandleInterrupt);
    
    OUTF("Waiting for board response")
    ensureSuccess(
        boardInterface.awaitBoardResponse(),
        "Board is not responding."
    );

    boardInterface.activateChannels(messenger.getEnabledChannels());

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
    exit(EXIT_SUCCESS);
}