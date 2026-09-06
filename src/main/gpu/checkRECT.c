/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"

typedef struct GpuRectDebugState {
    unsigned char level;
    unsigned char padding;
    short width;
    short height;
} GpuRectDebugState;

extern GpuRectDebugState D_8009574E;
extern void (*D_80095748)();
extern char D_800118B8[], D_800118A4[];

void checkRECT(char *name, RECT *rect) {
    char *format;
    switch (D_8009574E.level) {
    case 1:
        if (rect->w > D_8009574E.width || rect->w + rect->x > D_8009574E.width ||
            rect->y > D_8009574E.height || rect->y + rect->h > D_8009574E.height ||
            rect->w <= 0 || rect->x < 0 || rect->y < 0 || rect->h <= 0) {
            /* USA D_80011898: keep page and string offset separate. */
            format = (char *)0x80010000;
            asm volatile("" : "=r"(format) : "0"(format));
            format += 0x1898;
            break;
        }
        return;
    case 2:
        format = D_800118B8;
        break;
    default:
        return;
    }
    D_80095748(format, name);
    {
        int x = rect->x;
        int y = rect->y;
        int width = rect->w;
        register int height asm("$3") = rect->h;
        D_80095748(D_800118A4, x, y, width, height);
    }
}
