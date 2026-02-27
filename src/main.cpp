# include "interaction/arguments.hpp"
# include "interaction/port_selection.hpp"
# include "neuropawn/knight.hpp"
# include "utils/print_helpers.hpp"


int main(int argc, char* argv[])
{
    CommandLineArguments arguments = ParseCommandLineArguments(argc, argv);

    if (arguments.montage.empty())
    {
        PRINT("Montage not specified, stream will be missing channel label metadata.");
    }
    const char* port_name = selectPort(arguments.serialPort);

    serial::CSerialPort port;
    KnightProtocolParser parser;
    port.setProtocolParser(&parser);

    port.init(
        port_name, 115200,
        serial::ParityNone,
        serial::DataBits8,
        serial::StopTwo
    );
    if (port.open()) PRINTF("Opened serial port {}.", port_name)
    else
    {
        PRINTERR("Failed to open serial port.")
        exit(1);
    }

    for (;;) { sleep(100); }
}