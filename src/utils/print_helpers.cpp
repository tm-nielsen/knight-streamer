# include "utils/print_helpers.hpp"


void clear_line()
{
    std::cout << "\r\x1b[0J";
}

void clear_lines(int line_count)
{
    OUTF("\x1b[{}A", line_count - 1);
    clear_line();
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