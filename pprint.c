#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "pprint.h"

char *ansii_colors[]={
    "\033[0m",    // reset
    "\033[0;31m", // red
    "\033[0;32m", // green
    "\033[0;33m", // yellow
    "\033[0;34m", // blue
    "\033[0;35m", // magenta
    "\033[0;36m", // cyan
    "\033[0;37m"  // white
};

char* colorize(const enum color col, const char *format, ...) {
    va_list args;
    va_start(args, format);
    size_t size=strlen(format)*2; // estimate
    char *buffer=(char *)malloc(size*sizeof(char));
    vsnprintf(buffer, size, format, args);
    va_end(args);
    
    char *result=(char *)malloc(strlen(buffer)*sizeof(char));
    strcpy(result, ansii_colors[col]);
    strcat(result, buffer);
    strcat(result, ansii_colors[RESET]);

    free(buffer);
    return result;
}

void purple() { printf("\033[0;35m"); }

void green() { printf("\033[0;32m"); }

void reset() { printf("\033[0m"); }

void clear() { printf("\033[H\033[J"); }
