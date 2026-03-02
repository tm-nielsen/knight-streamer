# include "utils/print_helpers.hpp"


void clear_line()
{
    std::cout << "\r\x1b[K";
}

void clear_lines(int line_count)
{
    std::cout << "\x1b[2K";
    for (int i = 1; i < line_count; i++)
        std::cout << "\x1b[1A" << "\x1b[2K";
    std::cout << "\r";
}


std::vector<std::string> splitString(std::string& input, char delimiter)
{
    std::istringstream stream(input);
    std::vector<std::string> parts;
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        parts.push_back(token);
    }
    return parts;
}