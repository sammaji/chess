#ifndef UTILS_H
#define UTILS_H

#include "../types/types.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static void enter_alt_screen(void) { system("tput smcup"); }
static void leave_alt_screen(void) { system("tput rmcup"); }

void printansii(_ansii_esc esc);

int get_board_indexp(point *pt);
int get_board_index(int row, int col);
bool isbound(int row, int col);
bool isboundp(point *p);
bool isocc(chstate *state, int row, int col);
bool isoccp(chstate *state, point *p);

void msg(chstate *state, char *msg);
void err(chstate *state, char *errmsg);

#endif
