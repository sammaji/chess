#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>

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
    int rkx; int rky; int gkx; int gky; // king postions
} chlogtl;

void msg(chlogtl *logtl, char *msg) {
    logtl->msgocc=true;
    strcpy(logtl->msg, msg);
}

void error(chlogtl *logtl, char *errmsg) {
    logtl->errocc=true;
    strcpy(logtl->errmsg, errmsg);
}

int max(int x, int y) { return x>y ? x : y; }
int min(int x, int y) { return x<y ? x : y; }

int dist(point *p1, point *p2) {
    int a=p1->x-p2->x;
    int b=p1->y-p2->y;
    return (int)sqrt(a*a+b*b);
}

// returns largest component of dist vector
int stlgdist(point *p1, point *p2) {
    int x=abs(p1->x-p2->x);
    int y=abs(p1->y-p2->y);
    return x>y ? x : y;
}

// returns smallest component of dist vector
int stsmdist(point *p1, point *p2) {
    int x=abs(p1->x-p2->x);
    int y=abs(p1->y-p2->y);
    return x<y ? x : y;
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

bool isoccupied(chpiece** board, point *p) {
    return board[p->x][p->y]!=NIL;
}

bool ispurple(chpiece type) { return type>0 && type<=6; }
bool isgreen(chpiece type) { return type>6 && type<=12;}
bool getplayerfromtype(chpiece type) { return isgreen(type) ? GREEN: PURPLE; }
bool eqigncol(chpiece p1, chpiece p2) {
    p1=(p1>6) ? p1-6 : p1;
    return p1==p2;
}

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

void prompt(chlogtl *logtl) {
    printf(logtl->player==PURPLE ? "purple> " : "green> ");
}

void switchplayer(chlogtl *logtl) {
    logtl->player=(logtl->player==PURPLE)?GREEN:PURPLE;
}

void purple() { printf("\033[0;35m"); }
void green() { printf("\033[0;32m"); }
void reset() { printf("\033[0m"); }

void render_piece(chlogtl *logtl, chpiece piece) {
    wchar_t table[13]={L' ', L'♙', L'♔', L'♕', L'♗', L'♘', L'♖',
                        L'♙', L'♔', L'♕', L'♗', L'♘', L'♖'};
    if (piece==NIL) printf("%c", table[piece]);
    else if (piece<=6) {
        purple();
        wprintf(L"%lc", table[piece]);
    }
    else {
        green();
        wprintf(L"%lc", table[piece]);
    }
   reset();
}

void render_log(chlogtl *logtl) {
    if (logtl->msgocc) printf("MSG: %s\n", logtl->msg);
    if (logtl->errocc) printf("ERR: %s\n", logtl->errmsg);
    if (logtl->msgocc || logtl->errocc) printf("---\n\n");
    logtl->msgocc=false;
    logtl->errocc=false;
    strcpy(logtl->msg, "\0");
    strcpy(logtl->errmsg, "\0");
}

void render_board(chlogtl *logtl, chpiece** board) {
    printf("     ");
    for (int i=0; i<8; i++) {
        printf("%c   ", (char)(i+'a'));
    }
    printf("\n");
    
    printf("   ");
    for (int i=0; i<8; i++) {
        printf("+---");
    }
    printf("+\n");
    
    for (int i=7; i>=0; i--) {
        printf(" %d | ", i+1);
        for (int j=0; j<8; j++) {
            render_piece(logtl, board[i][j]);
            printf(" | ");
        }
        printf("%d\n   ", i+1);
        for (int j=0; j<8; j++) {
            printf("+---");
        }
        printf("+\n");
    }
    
    printf("     ");
    for (int i=0; i<8; i++) {
        printf("%c   ", (char)(i+'a'));
    }
    printf("\n");
}

void showpt(point* p) {
    printf("point: x=%d, y=%d\n", p->x, p->y);
}

point* readmov(chlogtl *logtl) {
    point *p=(point *)malloc(sizeof(point *));
    char inp[3];
    scanf("%2s", inp);
    if (inp[0]<'a' || inp[0]>'h' || inp[1]<'1' || inp[1]>'8') {
        error(logtl, "Invalid input, make sure you didn't make a typo");
    }
    p->y=inp[0]-'a';
    p->x=inp[1]-'1';
    return p;
}

void clear() {
    printf("\033[H\033[J");
}

void render(chlogtl *logtl, chpiece** board);

bool parsecmd(chlogtl *logtl, chpiece** board) {
    char ch;
    scanf("%c", &ch);
    if (ch=='q') logtl->exit=true;
    if (ch=='?') {
        msg(logtl, "Here's a list of commands:\n\nq - quit\n? - help\n> - possible moves (eg, >b2)");
        render(logtl, board);
        return true;
    }
    else ungetc(ch, stdin);
    return false;
}

void render(chlogtl *logtl, chpiece** board) {
    clear();
    
    if (logtl->exit==true) return;
    
    render_log(logtl);
    render_board(logtl, board);
    printf("\n");
    
    prompt(logtl);
    bool cmdocc=parsecmd(logtl, board);
    if (logtl->exit==true) return;
    if (cmdocc==true) return;
    
    point *from=readmov(logtl);
    getchar();
    point *to=readmov(logtl);
    
    bool valid=checkmov(logtl, board, from, to);
    if (valid) {
        pmovpiece(board, from, to);
        logtl->movc++;
        switchplayer(logtl);
    }
    render(logtl, board);
}

void init(chlogtl *logtl, chpiece** board) {
    // pawns
    int i=1;
    for (int j=0; j<8; j++) {
        addpiece(board, RPAWN, i, j);
    }
    addpiece(board, RKING, logtl->rkx, logtl->rky);
    addpiece(board, RQUEEN, 0, 4);
    addpiece(board, RBISHOP, 0, 2);
    addpiece(board, RBISHOP, 0, 5);
    addpiece(board, RKNIGHT, 0, 1);
    addpiece(board, RKNIGHT, 0, 6);
    addpiece(board, RROOK, 0, 0);
    addpiece(board, RROOK, 0, 7);
    
    // pawns
    i=6;
    for (int j=0; j<8; j++) {
        addpiece(board, GPAWN, i, j);
    }
    addpiece(board, GKING, logtl->gkx, logtl->gky);
    addpiece(board, GQUEEN, 7, 3);
    addpiece(board, GBISHOP, 7, 2);
    addpiece(board, GBISHOP, 7, 5);
    addpiece(board, GKNIGHT, 7, 1);
    addpiece(board, GKNIGHT, 7, 6);
    addpiece(board, GROOK, 7, 0);
    addpiece(board, GROOK, 7, 7);
}

int main() {
    setlocale(LC_CTYPE, "");

    chpiece** board=zeros();
    
    chlogtl *logtl=(chlogtl *)malloc(sizeof(chlogtl));
    logtl->exit=false;
    logtl->movc=0;
    logtl->player=PURPLE;
    logtl->msgocc=false;
    logtl->errocc=false;
    logtl->msg=(char *)malloc(200*sizeof(char));
    logtl->errmsg=(char *)malloc(200*sizeof(char));
    strcpy(logtl->msg, "\0");
    strcpy(logtl->errmsg, "\0");

    logtl->rkx=0; logtl->rky=3;
    logtl->gkx=7; logtl->gky=4;

    msg(logtl, "Welcome to the game! Type ? for help.");
    init(logtl, board);
    render(logtl, board);

    return 0;
}
