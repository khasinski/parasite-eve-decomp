/* MASPSX_FLAGS: --expand-div */
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_object.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"
#include "pe1/render_camera.h"
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"

/* Initialize camera projection and save the active actor screen position. */
int Render_PrepareFrame(void) {
    struct {
        short vector[4];
        void *result;
        union { int word; short xy[2]; } projected;
    } local;
    unsigned int *state = D_800B89F8;
    Render_InitViewState(state, state + 8);
    if (Geo_RenderMeshList((void *)D_800B0E40, &local.result)) return -2;
    SetGeomScreen(state[8]);
    func_800655D4();
    if (D_8009D254) {
        int x = D_8009D254->posX.parts.integer;
        int y, z;
        local.vector[0] = x;
        y = D_8009D254->posY.parts.integer;
        local.vector[1] = y;
        z = D_8009D254->posZ.parts.integer;
        local.vector[2] = z;
    } else local.vector[0] = local.vector[1] = local.vector[2] = 0;
    {
        /* Narrow volatile input preserves the retail stack frame in GCC 2.7.2. */
        register volatile unsigned short cx asm("$6") = 160;
        register unsigned int cy asm("$7") = 112;
        /* C offset shifts; pins and the empty constraint retain scheduling. */
        {
            register u32 ofx asm("$12");
            register u32 ofy asm("$13");
            asm("" : "=r"(cx), "=r"(cy) : "0"(cx), "1"(cy));
            ofx = (u32)cx << 16;
            ofy = cy << 16;
            gte_ctc2_24(ofx);
            gte_ctc2_25(ofy);
        }
    }
    {
        GteMatrix **address = &D_800BCFA4.value;
        asm("" : "=r"(address) : "0"(address));
        {
            register const GteMatrixWords *words asm("$6");
            words = (const GteMatrixWords *)*address;
            gte_ldrotmatrix(words);
            gte_ldtransmatrix(words);
        }
    }
    gte_lwc2_0_0(local.vector);
    gte_lwc2_1_4(local.vector);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    gte_stsxy2(&local.projected);
    {
        int ret = 0;
        int word;
        int x;
        word = local.projected.word;
        x = local.projected.xy[0];
        D_800BCFB4 = x;
        D_800BCFB6 = word >> 16;
        return ret;
    }
}

int Scene_CheckBattleFlag(void);
int Scene_IsBattleMode(void);
int Geo_BuildMeshList(void);
int Render_StepFade(void);
int Render_ApplyScreenTint(void);

int Render_Update(void) {
    Render_DrawSprite();
    Scene_CheckBattleFlag();
    Scene_IsBattleMode();
    Geo_BuildMeshList();
    Render_StepFade();
    Render_ApplyScreenTint();
    return 0;
}

/* Reset the screen tint state embedded in the scroll state: white target,
 * black start colour, two full-screen tiles and their draw-mode packets. */
s32 CdRom_InitScreenState(void) {
    s32 i;
    s32 value3;
    s32 value60;
    s32 value140;
    s32 valueE0;
    s32 value1;
    s32 command;
    register u8 *base asm("$6");
    register u8 *p asm("$4");
    register u8 *q asm("$5");
    register s32 tmp2 asm("$2");
    s32 tmp3;
    i = 0;
    value3 = 3;
    value60 = 0x60;
    value140 = 0x140;
    valueE0 = 0xE0;
    value1 = 1;
    command = 0xE1000400;
    base = (u8 *)&D_800BCF88.state;
    q = base;
    p = base;

    D_800BCF88.state.position.tint.target_b = 0xFF;
    D_800BCF88.state.position.tint.target_g = 0xFF;
    D_800BCF88.state.position.tint.target_r = 0xFF;
    D_800BCF88.state.position.tint.fade_mode = 1;
    D_800BCF88.state.position.tint.start_b = 0;
    D_800BCF88.state.position.tint.start_g = 0;
    D_800BCF88.state.position.tint.start_r = 0;
    D_800BCF88.state.position.tint.blend_mode = 2;

    do {
        p[0x33] = value3;
        p[0x37] = value60;
        tmp2 = *(u16 *)(base + 0x60);
        p[0x34] = tmp2;
        tmp2 = *(u16 *)(base + 0x62);
        p[0x35] = tmp2;
        tmp3 = *(u16 *)(base + 0x64);
        i++;
        *(u16 *)(p + 0x38) = 0;
        *(u16 *)(p + 0x3A) = 0;
        *(u16 *)(p + 0x3C) = value140;
        *(u16 *)(p + 0x3E) = valueE0;
        p[0x37] |= 2;
        p[0x36] = tmp3;
        q[0x53] = value1;
        tmp2 = base[0x67];
        p += 0x10;
        tmp2 &= 3;
        tmp2 <<= 5;
        tmp2 |= command;
        *(s32 *)(q + 0x54) = tmp2;
        q += 8;
    } while (i < 2);

    tmp2 = 0;
    asm volatile("" : : "r"(tmp2));
    *(u16 *)(base + 0x6E) = 0;
    *(u16 *)(base + 0x70) = 0;
    return tmp2;
}

/* Re-evaluate the ordering pointer after the packet-tag write. */
#define linkPrimitive(ordering, tag) { \
    ((RenderGpuTag *)(tag))->address = ((RenderGpuTag *)(ordering))->address; \
    ((RenderGpuTag *)(ordering))->address = (u32)(tag); \
}

int Render_SetCDDCSlot(void) {
    GeomScrollState *state = &D_800BCF88.state;
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
