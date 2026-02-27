# include "version.h"
# include "interaction/arguments.hpp"
# include "utils/print_helpers.hpp"


CommandLineArguments ParseCommandLineArguments(int argc, char* argv[])
{
    CommandLineArguments arguments;
    argparse::ArgumentParser parser("NeuroPawn Knight Pipe", VERSION);

    parser.add_description(
        "A simple command line interface to direct data "
        "from the NeuroPawn Knight board through lsl."
    );

    parser.add_argument("-p", "--port", "--port-name", "--serial-port")
        .help("Serial port address of the target device")
        .store_into(arguments.serialPort);

    parser.add_argument("-g", "--gain", "--eeg-gain")
        .help("EEG gain multiplier")
        .choices(1, 2, 3, 4, 6, 8, 12)
        .default_value(12)
        .store_into(arguments.gain);

    parser.add_argument("-i", "--imu", "--imu-board", "--use-imu-protocol")
        .help("Interpret data stream using the eeg + imu protocol")
        .default_value(false).implicit_value(true)
        .store_into(arguments.useIMUProtocol);


    parser.add_group("LSL Stream Parameters");

    parser.add_argument("-n", "--stream-name")
        .help("Name of the lsl stream through which to stream")
        .default_value("Knight_EEG")
        .store_into(arguments.streamName);

    parser.add_argument("-m", "--montage")
        .help("Channel labels in a comma-delimited list")
        .store_into(arguments.montage);

    try
    {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& err)
    {
        PRINTERR(err.what());
        std::exit(1);
    }
    return arguments;
}