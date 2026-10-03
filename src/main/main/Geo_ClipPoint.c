#include "pe1/geom_state.h"
#include "pe1/psyq_nop.h"

int Geo_ClipPoint(int x, int y, int z) {
    GeomState *state = g_GeomState;
    register int coordinate asm("$3");
    register int bound asm("$2");
    int clippedX;
    int clippedY;
    register int depth asm("$4");
    int savedBound;
    /* Retail reserves 32 bytes even though the function makes no calls.
     * Pins and identity barriers preserve its separate comparison/copy values. */
    int matchingStackReserve[8];

    coordinate = state->clip_offset_x + x;
    asm("" : "=r"(clippedX) : "0"(coordinate));
    bound = state->clip_offset_y;
    clippedY = bound + y;
    coordinate = (s16)coordinate;
    depth = state->depth_offset;

    bound = state->clip_min_x;
    /* Preserve the original empty load-delay slot. */
    PE1_NOP();
    asm("" : "=r"(savedBound) : "0"(bound));
    depth = depth + z;
    if (coordinate < bound) {
        clippedX = savedBound;
    } else {
        bound = state->clip_max_x;
        asm("" : "=r"(savedBound) : "0"(bound));
        if (bound < coordinate) {
            clippedX = savedBound;
        }
    }

    coordinate = clippedY << 16;
    bound = state->clip_min_y;
    coordinate = coordinate >> 16;
    asm("" : "=r"(savedBound) : "0"(bound));
    if (coordinate < bound) {
        clippedY = savedBound;
    } else {
        bound = state->clip_max_y;
        asm("" : "=r"(savedBound) : "0"(bound));
        if (bound < coordinate) {
            clippedY = savedBound;
        }
    }

    state->disp_src_x = clippedX;
    state->disp_src_y = clippedY;
    state->field26 = depth;
    return 0;
}
