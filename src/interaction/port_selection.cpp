# include "interaction/port_selection.hpp"
# include "utils/io_helpers.hpp"


const std::string selectPort(std::string argumentValue)
{
    if (argumentValue.empty())
    {
        PRINT("Serial Port not specified.");
        PRINT("---");
    }
    std::string portName = argumentValue;
    while (portName.empty())
    {
        PRINT("0)\trescan");

        auto availableDevices = serial::CSerialPortInfo::availablePortInfos();
        int deviceCount = (int)availableDevices.size();

        int deviceIndex = 1;
        for (const serial::SerialPortInfo& device : availableDevices)
        {
            PRINTF(
                "{})\t{} : {}", deviceIndex++,
                device.portName, device.description
            );
        }
        PRINTF("{})\texit", deviceIndex);
        OUTF("Which do you want to use? [0-{}]\t", deviceIndex);

        try
        {
            std::string input;
            std::cin >> input;
            int selection = std::stoi(input);

            if (selection == deviceCount + 1) exit(EXIT_SUCCESS);
            if (selection < 0 || selection > deviceCount + 1)
            {
                throw std::exception("invalid input");
            }
            else if (selection > 0)
            {
                portName = availableDevices[selection - 1].portName;
            }
        }
        catch (const std::exception& err)
        {
            PRINT("Invalid selection, exiting...")
            (void)err; exit(EXIT_SUCCESS);
        }

        if (portName.empty()) clear_lines(deviceCount + 4);
    }
    return portName;
}