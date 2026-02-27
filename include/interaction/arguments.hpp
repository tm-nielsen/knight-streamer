# pragma once


struct CommandLineArguments {
    std::string serialPort = "";
    int gain = 12;
    bool useIMUProtocol = false;

    std::string streamName = "";
    std::string montage = "";
};

CommandLineArguments ParseCommandLineArguments(int argc, char* argv[]);