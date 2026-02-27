# pragma once
# ifndef PRINT_MACROS
# define PRINT_MACROS

# ifndef OUT
# define OUT(line) std::cout << line;
# endif
# define OUTF(format_string, ...) std::cout << std::format(format_string, __VA_ARGS__);
# define PRINT(line) std::cout << line << std::endl;
# define PRINTF(format_string, ...) std::cout << std::format(format_string, __VA_ARGS__) << std::endl;
# define PRINTERR(line) std::cerr << line << std::endl;
# define CLEAR_PRINT() std::cout << "\x1b[1A" << "\x1b[2K\r";
# define STR(in) (std::ostringstream() << in).str();

void clear_line();
void clear_lines(int line_count);

# endif