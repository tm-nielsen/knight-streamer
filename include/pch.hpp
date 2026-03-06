# pragma once
# ifndef PCH_H
# define PCH_H

# include <iostream>
# include <thread>
# include <functional>
# include <chrono>
# include <string>
# include <cstdio>
# include <cstring>
# include <cmath>
# include <signal.h>
# include <stdarg.h>
# include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#define sleep(milliseconds) Sleep(milliseconds)
#else
#include <unistd.h>
#define sleep(milliseconds) usleep(1000 * milliseconds)
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