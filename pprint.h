#ifndef pprint_h
#define pprint_h

enum color {
    RESET=0,
    RED,
    GRN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

char* colorize(const enum color col, const char *format, ...);

void purple();

void green();

void reset();

void clear();

#endif
