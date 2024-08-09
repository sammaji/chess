#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include "chutils.h"
#include "rules.h"
#include "mathutils.h"

bool checkown(chlogtl *logtl, int type) {
    return (logtl->player==PURPLE&&type<=6) || (logtl->player==GREEN&&type>6);
}

bool checkbound(point *p) {
    return (p->x>=0 && p->x<8) && (p->y>=0 && p->y<8);
}

// check opacity, i.e,
// check if piece can move over pieces
bool checkopac(chlogtl *logtl, chpiece** board, point* from, point* to) {
    chpiece type=board[from->x][from->y];
    if (type==RKNIGHT||type==GKNIGHT) return true;
    
    // straight movement
    int dstsm=stsmdist(from, to);
    int dstlg=stlgdist(from, to);
    if (dstsm==0) {
        bool ismovx=(from->x-to->x)!=0;
        int f=(ismovx==true) ? from->x : from->y;
        int t=(ismovx==true) ? to->x : to->y;
        int s=min(f, t)+1;
        int e=max(f, t)-1;
        while(s<e) {
            int x=(ismovx==true) ? s : from->x;
            int y=(ismovx==true) ? from->y : s;
            point *p=(point *)malloc(sizeof(point));
            p->x=x; p->y=y;
            if (isoccupied(board, p)) return false;
        }
    }

    // diagonal moves
    if (dstlg==dstsm) {
        int sx=min(from->x, to->x)+1;
        int sy=min(from->y, to->y)+1;
        int ex=max(from->x, to->x)-1;
        int ey=max(from->y, to->y)-1;
        while (sx<=ex && sy<=ey) {
            point *p=(point *)malloc(sizeof(point));
            p->x=sx; p->y=sy;
            if (isoccupied(board, p)) return false;
            sx++; sy++;
        }
    }
    return true;
}

bool checkckmt(chlogtl *logtl, chpiece** board) {
    int kposx=(logtl->player==PURPLE) ? logtl->rkx : logtl->gkx;
    int kposy=(logtl->player==PURPLE) ? logtl->rky : logtl->gky;
    point *pos=(point *)malloc(sizeof(point));
    
    // from pawn
    for(int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            if (i==0 && j==0) continue;
            pos->x=kposx+i;
            pos->y=kposy+j;
            if (checkbound(pos)==false) continue;
            chpiece piece=board[pos->x][pos->y];
            bool issafe=(logtl->player==PURPLE && ispurple(piece))
                || (logtl->player==GREEN && isgreen(piece));
            if (piece==NIL || issafe) continue;
            if (piece==RROOK || piece==GROOK) return true;
        }
    }

    // from knight
    int dirknight[8][2]={
        {+1,+2}, {+1,-2}, {-1,+2}, {-1,-2},
        {+2,+1}, {+2,-1}, {-2,+1}, {-2,-1}
    };
    for (int i=0; i<8; i++) {
        pos->x=kposx+dirknight[i][0];
        pos->y=kposy+dirknight[i][1];
        if (checkbound(pos)==false) continue;
        chpiece piece=board[pos->x][pos->y];
        bool issafe=(logtl->player==PURPLE && ispurple(piece))
            || (logtl->player==GREEN && isgreen(piece));
        if (piece==NIL || issafe) continue;
        if (piece==RKNIGHT || piece==GKNIGHT) return true;
    }

    // from rook, bishop or queen
    int dir[8][2]={
        {1,1}, {1,-1}, {-1,1}, {-1,-1}, // diag paths
        {1,0}, {-1,0}, {0,1}, {0,-1}    // st paths
    };
    for (int i=0; i<8; i++) {
        pos->x=kposx; pos->y=kposy;
        do {
            pos->x+=dir[i][0]; // TODO: REFACTOR THIS BLOCK
            pos->y+=dir[i][1];
            if (checkbound(pos)==false) continue; 
            chpiece piece=board[pos->x][pos->y];
            bool issafe=(logtl->player==GREEN&&isgreen(piece)) || (logtl->player==PURPLE&&ispurple(piece));
            if (issafe) break; // one same color piece is blocking the path
            if (piece==NIL) continue;
            if (piece==RQUEEN || piece==GQUEEN) return true;
            else if (i<4 && (piece==RBISHOP || piece==GBISHOP)) return true;
            else if (i>=4 && (piece==RROOK || piece==GROOK)) return true;
        } while(checkbound(pos));
    }
    return false;
}

bool checkpawn(chlogtl *logtl, chpiece** board, point* from, point* to) {
    int dst=stlgdist(from,to);
    chpiece type=board[from->x][from->y];
    bool isvalidtype=(from->x==1 && type==RPAWN) || (from->x==6 && type==GPAWN);
    if (dst==2 && !isoccupied(board, to) && isvalidtype) return true;
    
    int d=dist(from,to);
    if (d==1 && !isoccupied(board, to)) return true;
    
    int diagmov[2][2]={{1,1}, {1,-1}};
    for (int i=0; i<2; i++) {
        int sign=(type==RPAWN) ? +1: -1;
        int x=from->x+(diagmov[i][0]*sign);
        int y=from->y+(diagmov[i][1]*sign);
        if (to->x==x && to->y==y && isoccupied(board, to)) return true;
    }
    
    error(logtl, "Invalid move for pawn");
    return false;
}

bool checkknight(chlogtl *logtl, chpiece** board, point* from, point *to) {
    int dstsm=stsmdist(from, to);
    int dstlg=stlgdist(from, to);
    bool valid=dstsm==1 && dstlg==2;
    if (valid==false) {
        error(logtl, "Invalid move for knight");
        return false;
    }
    return true;
}

bool checkrook(chlogtl *logtl, chpiece** board, point* from, point* to) {
    int dstsm=stsmdist(from,to);
    if (dstsm==0) return true;
    error(logtl, "Invalid move for rook");
    return false;
}

bool checkbishop(chlogtl *logtl, chpiece** board, point* from, point* to) {
    int dstsm=stsmdist(from,to);
    int dstlg=stlgdist(from,to);
    if (dstsm==dstlg) return true;
    error(logtl, "Invalid move for bishop");
    return false;
}

bool checkqueen(chlogtl *logtl, chpiece** board, point* from, point* to) {
    int dstsm=stsmdist(from,to);
    int dstlg=stlgdist(from,to);
    if (dstsm==0) return true;
    if (dstsm==dstlg) return true;
    error(logtl, "Invalid move for queen");
    return false;
}

bool checkking(chlogtl *logtl, chpiece** board, point* from, point* to) {
    int dstsm=stsmdist(from,to);
    int dstlg=stlgdist(from,to);
    if (dstsm==dstlg && dstsm==1) return true;
    error(logtl, "Invalid move for king");
    return false;
}

bool checkmov(chlogtl *logtl, chpiece** board, point* from, point* to) {
    if (!checkbound(from) || !checkbound(to)) {
        error(logtl, "Invalid move");
        return false;
    }
    
    if (from->x==to->x && from->y==to->y) {
        error(logtl, "You have to move a piece");
        return false;
    }
    
    chpiece type=board[from->x][from->y];
    if (type==NIL) {
        error(logtl, "That position is empty");
        return false;
    }
    
    bool own=checkown(logtl, board[from->x][from->y]);
    if (own==false) {
        error(logtl, "You don't own this piece");
        return false;
    }
    
    bool opac=checkopac(logtl, board, from, to);
    if (opac==false) {
        error(logtl, "Can't reach that position");
        return false;
    }
    
    chpiece typeto=board[to->x][to->y];
    if (logtl->player==PURPLE && ispurple(typeto)) {
        error(logtl, "That postion already has a purple piece");
        return false;
    }
    
    if (logtl->player==GREEN && isgreen(typeto)) {
        error(logtl, "That postion already has a green piece");
        return false;
    }

    switch (type) {
        case RPAWN:
        case GPAWN:
            bool ckpawn=checkpawn(logtl, board, from, to);
            if (ckpawn==false) return false;
            break;

        case RKNIGHT:
        case GKNIGHT:
            bool ckknight=checkknight(logtl, board, from, to);
            if (ckknight==false) return false;
            break;
            
        case RROOK:
        case GROOK:
            bool ckrook=checkrook(logtl, board, from, to);
            if (ckrook==false) return false;
            break;

        case RBISHOP:
        case GBISHOP:
            bool ckbishop=checkbishop(logtl, board, from, to);
            if (ckbishop==false) return false;
            break;
            
        case RQUEEN:
        case GQUEEN:
            bool ckqueen=checkqueen(logtl, board, from, to);
            if (ckqueen==false) return false;
            break;
            
        case RKING:
        case GKING:
            bool ckking=checkking(logtl, board, from, to);
            if (ckking==false) return false;
            break;

        case NIL:
            error(logtl, "That place is empty");
            return false;
    }

    bool ckmt=checkckmt(logtl, board);
    if (ckmt==true) {
        chpiece p=board[to->x][to->y];
        pmovpiece(board, from, to);
        bool nextckmt=checkckmt(logtl, board);
        pmovpiece(board, to, from);
        board[to->x][to->y]=p;
        if (nextckmt==true) {
            error(logtl, "Invalid move, you have a checkmate!");
            return false;
        }
    }
    return true;
}


