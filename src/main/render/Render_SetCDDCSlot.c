/* MASPSX_FLAGS: --expand-div */
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"

/* Re-evaluate the ordering pointer after the packet-tag write. */
#define linkPrimitive(ordering, tag) { \
    ((RenderGpuTag *)(tag))->address = ((RenderGpuTag *)(ordering))->address; \
    ((RenderGpuTag *)(ordering))->address = (u32)(tag); \
}

int Render_SetCDDCSlot(void) {
    GeomScrollState *state = &D_800BCF88;
    int mode = D_800BCFEE & 3;
    int stop = D_800BCFEE & 4;
    int divisor, frame;
    int slot;
    int modeSlot;
    RenderBufferPrefix *buffers;

    if (!mode) {
        return 0;
    }
    if (mode == 2) {
        divisor = D_800BCFF6 - 1;
        if (divisor <= 0) {
            divisor = 1;
        }
        frame = D_800BCFF8;
        state->position.tint.tiles[D_8009CDDC].r =
            D_800BCFF0 + (D_800BCFE8 - D_800BCFF0) * frame / divisor;
        state->position.tint.tiles[D_8009CDDC].g =
            D_800BCFF2 + (D_800BCFEA - D_800BCFF2) * frame / divisor;
        state->position.tint.tiles[D_8009CDDC].b =
            D_800BCFF4 + (D_800BCFEC - D_800BCFF4) * frame / divisor;
    } else {
        state->position.tint.tiles[D_8009CDDC].r = D_800BCFE8;
        state->position.tint.tiles[D_8009CDDC].g = D_800BCFEA;
        state->position.tint.tiles[D_8009CDDC].b = D_800BCFEC;
    }
    buffers = &D_800B0E38;
    slot = D_8009CDDC;
    linkPrimitive((u32 *)buffers->ordering[slot] + 3,
                  &state->position.tint.tiles[slot].tag);
    (slot + state->position.tint.modes)->tag.length = 1;
    modeSlot = D_8009CDDC;
    (modeSlot + state->position.tint.modes)->command =
        0xe1000400 | ((state->position.tint.blend_mode & 3) << 5);
    linkPrimitive((u32 *)buffers->ordering[modeSlot] + 3,
                  &state->position.tint.modes[modeSlot].tag);
    if (mode == 2) {
        state->position.tint.frame++;
        if (state->position.tint.frame >= state->position.tint.duration) {
            if (stop) {
                state->position.tint.fade_mode = 0;
            } else {
                state->position.tint.fade_mode = 1;
            }
        }
    }
    return 0;
}
