#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "chutils.h"

void msg(chlogtl *logtl, char *msg) {
    logtl->msgocc=true;
    strcpy(logtl->msg, msg);
}

void error(chlogtl *logtl, char *errmsg) {
    logtl->errocc=true;
    strcpy(logtl->errmsg, errmsg);
}

chpiece** zeros() {
    chpiece** arr=(chpiece **)malloc(8*sizeof(chpiece *));
    for (int i=0; i<8; i++) {
        arr[i]=(chpiece *)calloc(8, sizeof(chpiece));
    }
    return arr;
}

void addpiece(chpiece** board, chpiece type, int x, int y) {
    if (x>=8 || y>=8) return;
    if (board[x][y]>0) return;
    board[x][y]=type;
}

void rmpiece(chpiece** board, int x, int y) {
    if (x>=8 || y>=8) return;
    board[x][y]=0;
}

void movpiece(chpiece** board, int fromx, int fromy, int tox, int toy) {
    if (fromx>=8 || fromy>=8 || tox>=8 || toy>=8) return;
    if (board[fromx][fromy]<=0) return; // can't erase tox,toy
    if (board[tox][toy]>0) rmpiece(board, tox, toy); // for logs (maybe)

    // TODO: mark the king
    if (board[fromx][fromy]==RKING) {
        
    }

    board[tox][toy]=board[fromx][fromy];
    rmpiece(board, fromx, fromy); 
}

void pmovpiece(chpiece** board, point* from, point* to) {
    movpiece(board, from->x, from->y, to->x, to->y);
}

bool isoccupied(chpiece** board, point *p) { return board[p->x][p->y]!=NIL; }

bool ispurple(chpiece type) { return type>0 && type<=6; }

bool isgreen(chpiece type) { return type>6 && type<=12;}

bool getplayerfromtype(chpiece type) { return isgreen(type) ? GREEN: PURPLE; }

bool eqigncol(chpiece p1, chpiece p2) {
    p1=(p1>6) ? p1-6 : p1;
    return p1==p2;
}
