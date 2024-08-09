#include "chutils.h"

#ifndef rules_h
#define rules_h

bool checkown(chlogtl *logtl, int type);

bool checkbound(point *p);

// check opacity, i.e,
// check if piece can move over pieces
bool checkopac(chlogtl *logtl, chpiece** board, point* from, point* to);

bool checkckmt(chlogtl *logtl, chpiece** board);

bool checkpawn(chlogtl *logtl, chpiece** board, point* from, point* to);

bool checkknight(chlogtl *logtl, chpiece** board, point* from, point *to);

bool checkrook(chlogtl *logtl, chpiece** board, point* from, point* to);

bool checkbishop(chlogtl *logtl, chpiece** board, point* from, point* to);

bool checkqueen(chlogtl *logtl, chpiece** board, point* from, point* to);

bool checkking(chlogtl *logtl, chpiece** board, point* from, point* to);

bool checkmov(chlogtl *logtl, chpiece** board, point* from, point* to);

#endif
