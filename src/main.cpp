# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "neuropawn/knight.hpp"
# include "transmission/eeg_messenger.hpp"
# include "utils/print_helpers.hpp"


void HandleInterrupt(int);
static volatile bool interrupted = false;


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
    
    signal(SIGINT, HandleInterrupt);
    signal(SIGTERM, HandleInterrupt);

    
    KnightProtocolParser parser = arguments.useIMUProtocol
        ? KnightIMUProtocolParser(arguments.gain)
        : KnightProtocolParser(arguments.gain);
    EEGMessenger messenger {arguments.streamName};

    parser.setListener(&messenger);
    port.setProtocolParser(&parser);

    PRINT("---");
    PRINTF("Opened serial port {}.", portName)

    sleep(2000);

    bool configResult = true;
    for (int i = 1; i < CHANNEL_COUNT + 1; i++)
    {
        configResult &= port.writeData(std::format("chon_{}_12", i).c_str(), 10) == 10;
        sleep(1);
        configResult &= port.writeData(std::format("rldadd_{}", i).c_str(), 9) == 9;
        sleep(1);
        if (!configResult)
        {
            PRINTERR("Failed to configure board.");
            port.close();
            exit(1);
        }
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