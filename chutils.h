#include <stdbool.h>

#ifndef chutils_h
#define chutils_h

typedef struct point { int x; int y; } point;

typedef enum chpiece {
    NIL=0,
    RPAWN, RKING, RQUEEN, RBISHOP, RKNIGHT, RROOK,
    GPAWN, GKING, GQUEEN, GBISHOP, GKNIGHT, GROOK
} chpiece;

typedef enum chplayer { PURPLE, GREEN } chplayer;

typedef struct chlogtl {
    bool exit;
    bool msgocc;
    bool errocc;
    char *msg;
    char *errmsg;
    int movc;
    chplayer player;
    bool player_ini_state;
    bool player_final_state;
    int rkx; int rky; int gkx; int gky; // king postions
    point *from;
    point *to;
} chlogtl;

void msg(chlogtl *logtl, char *msg);

void error(chlogtl *logtl, char *errmsg);

chpiece** zeros();

void addpiece(chpiece** board, chpiece type, int x, int y);

void rmpiece(chpiece** board, int x, int y);

void movpiece(chpiece** board, int fromx, int fromy, int tox, int toy);

void pmovpiece(chpiece** board, point* from, point* to);

bool isoccupied(chpiece** board, point *p);

bool ispurple(chpiece type);

bool isgreen(chpiece type);

bool getplayerfromtype(chpiece type);

bool eqigncol(chpiece p1, chpiece p2);

#endif
