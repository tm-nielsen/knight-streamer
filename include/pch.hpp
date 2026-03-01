# pragma once
# ifndef PCH_H
# define PCH_H

# include <iostream>
# include <string>
# include <cstdio>
# include <cstring>
# include <cmath>
# include <signal.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define sleep(microsecond) usleep(1000 * microsecond) // ms
#endif


# include "lsl_cpp.h"
# include "argparse/argparse.hpp"
# include "CSerialPort/SerialPort.h"
# include "CSerialPort/SerialPortInfo.h"
# pragma warning(push, 0)
# include "CSerialPort/IProtocolParser.h"
# pragma warning(pop)

namespace serial = itas109;

# endif