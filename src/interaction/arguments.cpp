# include "version.h"
# include "interaction/arguments.hpp"
# include "neuropawn/knight_protocol_constants.h"
# include "utils/io_helpers.hpp"

using namespace KnightProtocolConstants;


CommandLineArguments ParseCommandLineArguments(int argc, char* argv[])
{
    CommandLineArguments arguments;
    std::string montageString;
    std::vector<std::string> channelLabels;

    argparse::ArgumentParser parser("NeuroPawn Knight Streamer", VERSION);

    parser.add_description(
        "A simple command line interface to direct data "
        "from the NeuroPawn Knight board through lsl."
    );

    parser.add_argument("-p", "--port", "--port-name", "--serial-port")
        .help("Serial port address of the target device")
        .store_into(arguments.serialPort);

    parser.add_argument("-g", "--gain", "--eeg-gain")
        .help("EEG gain multiplier")
        .default_value(12)
        .store_into(arguments.gain);

    parser.add_argument("-i", "--imu", "--imu-board", "--use-imu-protocol")
        .help("Interpret data stream using the eeg + imu protocol")
        .flag().store_into(arguments.useIMUProtocol);


    parser.add_group("LSL Stream Parameters");

    parser.add_argument("-n", "--stream-name")
        .help("Name of the lsl stream through which to stream")
        .default_value("Knight_EEG")
        .store_into(arguments.streamName);

    parser.add_argument("-m", "--montage", "--montage-string")
        .help(
            "Channel labels in a comma-delimited list.\n"
            "Example: \"PO8,PO4, ,POZ,OZ,PO7, ,P03\"\n"
            "Empty labels will disable the channel.\n" +
            std::format("All {} channels must be specified.\n", CHANNEL_COUNT)
        ).store_into(montageString);

    try
    {
        parser.parse_args(argc, argv);

        ValidateCommandLineArguments(arguments);
        arguments.channelLabels = ParseChannelLabels(montageString);
    }
    catch (const std::exception& err)
    {
        PRINTERR(err.what());
        exit(EXIT_SUCCESS);
    }
    return arguments;
}


const std::set<int> CommandLineArguments::validGainValues = {1, 2, 3, 4, 6, 8, 12};

void ValidateCommandLineArguments(CommandLineArguments arguments)
{
    if (!CommandLineArguments::validGainValues.count(arguments.gain))
    {
        throw std::exception(
            "Invalid gain value - allowed options: {1, 2, 3, 4, 6, 8, 12}"
        );
    }
}


std::vector<std::string> ParseChannelLabels(std::string montageString)
{
    removeCharacterInstances(montageString, ' ');

    if (montageString.empty())
    {
        PRINTF("Montage not specified, will assume {} active channels.", CHANNEL_COUNT);
        PRINT("Stream will be missing channel label metadata.");
        return {};
    }

    std::vector<std::string> channelLabels = splitString(montageString);
    if (channelLabels.size() != CHANNEL_COUNT)
    {
        throw std::exception(
            std::format(
                "Invalid montage - all {} channels must be labelled or disabled.",
                CHANNEL_COUNT
            ).c_str()
        );
    }

    for (int i = 0; i < KnightProtocolConstants::CHANNEL_COUNT; i++)
    {
        if (!channelLabels[i].empty()) return channelLabels;
    }
    throw std::exception("All channels disabled, nothing to stream.");
}