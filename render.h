#include "chutils.h"

#ifndef render_h
#define render_h

void prompt(chlogtl *logtl);

void switchplayer(chlogtl *logtl);

void render_piece(chlogtl *logtl, chpiece piece);

void render_log(chlogtl *logtl);

void render_board(chlogtl *logtl, chpiece** board);

void showpt(point* p);

bool readmov(chlogtl *logtl, char *inp, point *p);

void render(chlogtl *logtl, chpiece** board);

bool parsecmd(chlogtl *logtl, chpiece** board, char *inp);

void init(chlogtl *logtl, chpiece** board);

#endif
