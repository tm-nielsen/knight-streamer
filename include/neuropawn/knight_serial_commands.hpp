# pragma once


bool writeSerialCommand(serial::CSerialPort &port, std::string command);
bool enableKnightBoardEEGChannel(serial::CSerialPort &port, int channelIndex);