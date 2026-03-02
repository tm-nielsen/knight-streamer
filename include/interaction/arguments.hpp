# pragma once


struct CommandLineArguments {
    std::string serialPort = "";
    int gain = 12;
    bool useIMUProtocol = false;

    std::string streamName = "";
    std::string montageString = "";
};

const std::set<int> validGainValues = {1, 2, 3, 4, 6, 8, 12};

CommandLineArguments ParseCommandLineArguments(int argc, char* argv[]);