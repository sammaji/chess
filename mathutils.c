#include <stdlib.h>
#include <math.h>
#include "mathutils.h"
#include "chutils.h"

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
