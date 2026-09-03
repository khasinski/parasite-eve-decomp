#include "pe1/geom_state.h"

int Geo_TransformPoint(GeomEntry *entry, int x, int y, int depth) {
    GeomEntry *current;
    int screenX;
    register int screenY asm("$4");
    register int minimum asm("$5");
    int truncated;
    register int sum asm("$3");
    register unsigned int shifted asm("$3");
    int bound;
    int comparison;
    unsigned int depthWord;
    unsigned int packed;
    unsigned int mask;
    volatile int stackFrame[8]; /* Preserves the original 0x20-byte frame. */

    current = entry;
    sum = current->base_x;
    screenY = current->base_y;
    depthWord = *(unsigned int *)current;
    sum += x;
    screenX = sum;
    asm volatile("" : "=r"(sum) : "0"(sum));
    depthWord >>= 20;
    depth += depthWord;

    truncated = (short)sum;
    bound = current->ot10;
    minimum = bound;
    asm volatile("" : "=r"(bound) : "0"(bound));
    comparison = truncated < bound;
    screenY += y;
    if (comparison) {
        goto clamp_x;
    }
    bound = current->ot12;
    minimum = bound;
    asm volatile("" : "=r"(bound) : "0"(bound));
    comparison = bound < truncated;
    if (comparison) {
        goto clamp_x;
    }
    goto x_done;

clamp_x:
    screenX = minimum;

x_done:
    shifted = (unsigned int)screenY << 16;
    bound = current->ot14;
    truncated = (int)shifted >> 16;
    minimum = bound;
    asm volatile("" : "=r"(bound) : "0"(bound));
    comparison = truncated < bound;
    if (comparison) {
        goto clamp_y;
    }
    bound = current->ot16;
    minimum = bound;
    asm volatile("" : "=r"(bound) : "0"(bound));
    comparison = bound < truncated;
    if (comparison) {
        goto clamp_y;
    }
    goto y_done;

clamp_y:
    screenY = minimum;

y_done:
    current->scr_x = screenX;
    current->scr_y = screenY;
    mask = 0xFFF000FF;
    packed = *(unsigned int *)current & mask;
    packed |= (depth & 0xFFF) << 8;
    *(unsigned int *)current = packed;
    return 0;
}
