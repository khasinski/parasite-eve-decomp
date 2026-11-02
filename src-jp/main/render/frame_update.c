/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"
#include "pe1/game_state.h"
#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_buffers.h"
#include "pe1/render_camera.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_GridCursorIndex;
int g_GridCursorPrevious;
int g_GridCursorX;
int g_GridCursorY;

/* Each entry packs a grid position: x in bits 22..31, y in bits 12..21. */
#define ENTRY_X(value) ((value) >> 22)
#define ENTRY_Y(value) (((value) >> 12) & 0x3FF)

/* Moves the grid cursor over `entries`. A restart picks the last entry of
 * the bottom row (descending) or the first entry of the top row. Otherwise
 * the cursor steps one column (16 units) left or right; if that column is
 * empty it moves to the nearest entry of the same row, or to the nearest end
 * of the next row, and restarts when there is none. */
int Render_MoveGridCursor(PrimObj *obj, u32 *entries, u8 ascending, u8 restart) {
    int bestX = 0x7FFF;
    int best = 0;
    int first = 0;
    int bestY;
    int step;
    int i;
    int y;
    u8 found;
    u8 later;
    u32 value;
    int previous;

    if (restart) {
        if (!ascending) {
            bestY = -1;
            bestX = -1;
            for (i = 0; i < obj->count; i++) {
                y = ENTRY_Y(entries[i]);
                if (bestY < y)
                    bestY = y;
            }
            for (i = 0; i < obj->count; i++) {
                value = entries[i];
                if (ENTRY_Y(value) == bestY && bestX < (int)ENTRY_X(value)) {
                    bestX = ENTRY_X(value);
                    first = i;
                }
            }
            previous = g_GridCursorIndex;
            g_GridCursorIndex = first;
            g_GridCursorPrevious = previous;
            g_GridCursorX = ENTRY_X(entries[first]);
            g_GridCursorY = ENTRY_Y(entries[first]);
            return first;
        }
        bestY = 0x7FFF;
        for (i = 0; i < obj->count; i++) {
            y = ENTRY_Y(entries[i]);
            if (y < bestY)
                bestY = y;
        }
        for (i = 0; i < obj->count; i++) {
            value = entries[i];
            if (ENTRY_Y(value) == bestY && (int)ENTRY_X(value) < bestX) {
                bestX = ENTRY_X(value);
                best = i;
            }
        }
    } else {
        step = 16;
        if (!ascending)
            step = -16;
        found = 0;
        later = 0;
        for (i = 0; i < obj->count; i++) {
            value = entries[i];
            if (ENTRY_X(value) == g_GridCursorX + step) {
                if (ENTRY_Y(value) == g_GridCursorY) {
                    previous = g_GridCursorIndex;
                    g_GridCursorIndex = i;
                    g_GridCursorPrevious = previous;
                    g_GridCursorX = ENTRY_X(entries[i]);
                    g_GridCursorY = ENTRY_Y(entries[i]);
                    return i;
                }
            } else if (ENTRY_Y(value) == g_GridCursorY && g_GridCursorX < (int)ENTRY_X(value)) {
                later = 1;
            }
        }
        if (!found && later == 1) {
            if (!ascending) {
                bestX = 0;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY && (int)ENTRY_X(value) < g_GridCursorX &&
                        bestX < (int)ENTRY_X(value)) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            } else {
                bestX = 0x7FFF;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY && g_GridCursorX < (int)ENTRY_X(value) &&
                        (int)ENTRY_X(value) < bestX) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            }
        }
        if (found != 1) {
            found = 0;
            if (!ascending) {
                bestX = -1;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY + step && bestX < (int)ENTRY_X(value)) {
                        /* Save X through the scratch before installing the new index. */
                        best = ENTRY_X(value);
                        bestX = best;
                        best = i;
                        found = 1;
                    }
                }
            } else {
                bestX = 0x7FFF;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY + step && (int)ENTRY_X(value) < bestX) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            }
            if (!found)
                return Render_MoveGridCursor(obj, entries, ascending, 1);
        }
    }
    previous = g_GridCursorIndex;
    g_GridCursorIndex = best;
    g_GridCursorPrevious = previous;
    g_GridCursorX = ENTRY_X(entries[best]);
    g_GridCursorY = ENTRY_Y(entries[best]);
    return best;
}

/* Initialize camera projection and save the active actor screen position. */
int Render_PrepareFrame(void) {
    struct {
        short vector[4];
        void *result;
        union { int word; short xy[2]; } projected;
    } local;
    unsigned int *state = g_EntityRenderScratch;
    Render_InitViewState(state, state + 8);
    if (Render_DrawMeshList((void *)g_GameState.texture_load_scratch, &local.result)) return -2;
    SetGeomScreen(state[8]);
    Render_ResetAllEntries();
    if (D_8009CD10) {
        int x = D_8009CD10->posX.parts.integer;
        int y, z;
        local.vector[0] = x;
        y = D_8009CD10->posY.parts.integer;
        local.vector[1] = y;
        z = D_8009CD10->posZ.parts.integer;
        local.vector[2] = z;
    } else local.vector[0] = local.vector[1] = local.vector[2] = 0;
    {
        /* Matching debt: a volatile register input is the only form found
         * that keeps the retail 48-byte frame (the 8 bytes above the
         * projected point are never accessed). */
        register volatile unsigned short cx asm("$6") = 160;

        gte_SetGeomOffset(cx, 112);
    }
    {
        GteMatrix **slot = &g_GeomScroll.words.viewMatrix;
        register GteMatrix *view asm("$6");

        asm("" : "=r"(slot) : "0"(slot));
        view = *slot;
        gte_ldrotmatrix(view);
        gte_ldtransmatrix(view);
    }
    gte_ldv0(local.vector);
    gte_rtps();
    gte_stsxy2(&local.projected);
    {
        int ret = 0;
        int word = local.projected.word;
        int x = local.projected.xy[0];

        g_GeomScroll.state.position.cameraOffsetX = x;
        g_GeomScroll.state.position.cameraOffsetY = word >> 16;
        return ret;
    }
}

int Render_BuildMeshList(void);
int Render_StepFade(void);
int Render_ApplyScreenTint(void);

int Render_Update(void) {
    Render_DrawSprite();
    Render_StepGroupAnimations();
    Render_ScrollEntries();
    Render_BuildMeshList();
    Render_StepFade();
    Render_ApplyScreenTint();
    return 0;
}

/* Reset the screen tint state embedded in the scroll state: white target,
 * black start colour, two full-screen tiles and their draw-mode packets. */
s32 CdRom_InitScreenState(void) {
    GeomScrollState *state = &g_GeomScroll.state;
    s32 i;

    g_GeomScroll.state.position.tint.targetB = 0xFF;
    g_GeomScroll.state.position.tint.targetG = 0xFF;
    g_GeomScroll.state.position.tint.targetR = 0xFF;
    g_GeomScroll.state.position.tint.fadeMode = 1;
    g_GeomScroll.state.position.tint.startB = 0;
    g_GeomScroll.state.position.tint.startG = 0;
    g_GeomScroll.state.position.tint.startR = 0;
    g_GeomScroll.state.position.tint.blendMode = 2;
    for (i = 0; i < 2; i++) {
        state->position.tint.tiles[i].tag.length = 3;
        state->position.tint.tiles[i].command = 0x60;
        state->position.tint.tiles[i].r = state->position.tint.targetR;
        state->position.tint.tiles[i].g = state->position.tint.targetG;
        state->position.tint.tiles[i].b = state->position.tint.targetB;
        state->position.tint.tiles[i].x = 0;
        state->position.tint.tiles[i].y = 0;
        state->position.tint.tiles[i].width = 320;
        state->position.tint.tiles[i].height = 224;
        state->position.tint.tiles[i].command |= 2;
        state->position.tint.modes[i].tag.length = 1;
        state->position.tint.modes[i].command = 0xE1000400 | ((state->position.tint.blendMode & 3) << 5);
    }
    state->position.tint.duration = 0;
    state->position.tint.frame = 0;
    return 0;
}

/* A packet tag seen as a pointer and as the address word the DMA chain
 * links through. */
typedef union RenderTagAddress {
    RenderGpuTag *tag;
    u32 word;
} RenderTagAddress;

/* PSY-Q addPrim; the ordering pointer is re-evaluated after the tag write. */
#define linkPrimitive(ordering, packet) { \
    RenderTagAddress link; \
    (packet)->address = ((RenderGpuTag *)(ordering))->address; \
    link.tag = (packet); \
    ((RenderGpuTag *)(ordering))->address = link.word; \
}

int Render_SetCDDCSlot(void) {
    GeomScrollState *state = &g_GeomScroll.state;
    int mode = g_ScreenTint.fadeMode & 3;
    int stop = g_ScreenTint.fadeMode & 4;
    int divisor, frame;
    int slot;
    int modeSlot;
    RenderBufferPrefix *buffers;

    if (!mode) {
        return 0;
    }
    if (mode == 2) {
        divisor = g_ScreenTint.duration - 1;
        if (divisor <= 0) {
            divisor = 1;
        }
        frame = g_ScreenTint.frame;
        state->position.tint.tiles[g_ActiveDrawSlot].r =
            g_ScreenTint.startR + (g_ScreenTint.targetR - g_ScreenTint.startR) * frame / divisor;
        state->position.tint.tiles[g_ActiveDrawSlot].g =
            g_ScreenTint.startG + (g_ScreenTint.targetG - g_ScreenTint.startG) * frame / divisor;
        state->position.tint.tiles[g_ActiveDrawSlot].b =
            g_ScreenTint.startB + (g_ScreenTint.targetB - g_ScreenTint.startB) * frame / divisor;
    } else {
        state->position.tint.tiles[g_ActiveDrawSlot].r = g_ScreenTint.targetR;
        state->position.tint.tiles[g_ActiveDrawSlot].g = g_ScreenTint.targetG;
        state->position.tint.tiles[g_ActiveDrawSlot].b = g_ScreenTint.targetB;
    }
    buffers = &D_800B08F8;
    slot = g_ActiveDrawSlot;
    linkPrimitive(&buffers->ordering[slot][3],
                  &state->position.tint.tiles[slot].tag);
    (slot + state->position.tint.modes)->tag.length = 1;
    modeSlot = g_ActiveDrawSlot;
    (modeSlot + state->position.tint.modes)->command =
        0xe1000400 | ((state->position.tint.blendMode & 3) << 5);
    linkPrimitive(&buffers->ordering[modeSlot][3],
                  &state->position.tint.modes[modeSlot].tag);
    if (mode == 2) {
        state->position.tint.frame++;
        if (state->position.tint.frame >= state->position.tint.duration) {
            if (stop) {
                state->position.tint.fadeMode = 0;
            } else {
                state->position.tint.fadeMode = 1;
            }
        }
    }
    return 0;
}
