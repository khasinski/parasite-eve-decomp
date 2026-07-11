/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_gpu.h"

typedef unsigned char u8;

extern u8 D_8009574E[];
extern void (*D_80095748[])(char *msg, ...);
extern char D_80011898[];
extern char D_800118A4[];
extern char D_800118B8[];

void checkRECT(char *name, RECT *rect) {
    u8 *debug;
    int bad;

    debug = D_8009574E;
    if (debug[0] == 1) {
        bad = 0;
        if (*(short *)(debug + 2) < rect->w) {
            bad = 1;
        } else if (*(short *)(debug + 2) < rect->x + rect->w) {
            bad = 1;
        } else if (*(short *)(debug + 4) < rect->y) {
            bad = 1;
        } else if (*(short *)(debug + 4) < rect->y + rect->h) {
            bad = 1;
        } else if (rect->w <= 0) {
            bad = 1;
        } else if (rect->x < 0) {
            bad = 1;
        } else if (rect->y < 0) {
            bad = 1;
        } else if (rect->h <= 0) {
            bad = 1;
        }

        if (bad == 0) {
            return;
        }

        D_80095748[0](D_80011898, name);
    } else if (debug[0] == 2) {
        D_80095748[0](D_800118B8, name);
    } else {
        return;
    }

    D_80095748[0](D_800118A4, rect->x, rect->y, rect->w, rect->h);
}
