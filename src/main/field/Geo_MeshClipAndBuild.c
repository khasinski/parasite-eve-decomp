/* Field geometry: clamp the display-source point into the clip window and
 * draw the selected group's sprite entries relative to the camera.
 * Contiguous default-profile pair on g_GeomState (main_tu_evidence G0836
 * joins them with the mesh renderer before and the fades after). */
#include "pe1/geom_state.h"
#include "pe1/psyq_nop.h"

/* Camera y read through an array view: the scalar form schedules the load
 * before the preceding store. */
extern u16 g_CameraClampedY[];


void Render_DrawSpriteEntry(GeomEntry *entry);

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

s32 Geo_BuildMeshList(void) {
    register GeomStateAddress base asm("$4");
    GeomEntry *entry;
    s32 i;
    register u16 count asm("$18");
    s32 temp;
    register s32 value asm("$3");
    s32 stack_pad[2];

    base.state = g_GeomState;
    temp = (u16)D_800BCF8C.x;
    temp -= 0xA0;
    value = base.state->disp_src_x;
    count = base.state->entry_count06;
    value -= temp;
    base.state->out_disp_x = value;
    temp = g_CameraClampedY[0];
    value = base.state->disp_src_y;
    temp -= 0x70;
    value -= temp;
    temp = base.state->entry_offset;
    i = 0;
    base.state->out_disp_y = value;
    base.word += temp;
    if (count != 0) {
        entry = base.entry;
        do {
            if ((entry->flags & 2) && (entry->group == g_GeomGroupSel)) {
                Render_DrawSpriteEntry(entry);
            }
            i += 1;
            entry++;
        } while (i < count);
    }
    return 0;
}
