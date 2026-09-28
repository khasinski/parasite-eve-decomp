/* MASPSX_FLAGS: --expand-div */
/* Nonmatching candidate: 2 register-word differences at +0xE4 and +0xE8. */
#include "common.h"
#include "pe1/akao/pos.h"
#include "pe1/gte.h"

extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
#define GAME ((u8 *)&D_800B0CD8_o)

extern u16 D_800B0DD0;
extern u16 D_800B0DD2;
extern int *D_800BCFA4;
extern int *D_800BCFA8;

int Render_SetGteScreenOffset(void);
int Render_ResetGteScreenOffset(void);
int RotTransPers(void *v, void *sxy, void *p, int *flag);

#define AKAO_GTE_CTC2_26(value) asm volatile("ctc2 %0,$26" : : "r"(value))

static __inline__ void Akao_LoadRotMatrix(volatile int *matrix) {
    register volatile int *mat asm("$8") = matrix;
    register int r12 asm("$12");
    register int r13 asm("$13");
    register int r14 asm("$14");
    r12 = mat[0]; r13 = mat[1];
    gte_ctc2_0(r12);
    gte_ctc2_1(r13);
    r12 = mat[2]; r13 = mat[3]; r14 = mat[4];
    gte_ctc2_2(r12);
    gte_ctc2_3(r13);
    gte_ctc2_4(r14);
    r12 = mat[5]; r13 = mat[6];
    gte_ctc2_5(r12);
    r14 = mat[7];
    gte_ctc2_6(r13);
    gte_ctc2_7(r14);
}

static __inline__ void Akao_SetGeomScreenFromState(void) {
    int geom_screen;

    geom_screen = *D_800BCFA8;
    AKAO_GTE_CTC2_26(geom_screen);
}

s32 Akao_Calc3DPan(AkaoPackedRect3 *rect, s32 *out_pan, s32 *out_volume) {
    register u8 *game asm("$17");
    s32 coords[3];
    s32 otz;
    s32 screen_x;
    s32 pan;
    register s32 min_depth asm("$3");
    register s32 max_depth asm("$2");
    s32 depth_range;
    register s32 attenuation asm("$16");
    s32 squared;
    s32 min_volume;
    s32 volume_delta;
    s32 volume;

    Render_SetGteScreenOffset();
    game = GAME;
    asm volatile("" : : "r"(game));
    { int **matrix_holder = &D_800BCFA4;
      asm volatile("" : : "r"(matrix_holder));
      Akao_LoadRotMatrix(*matrix_holder);
    }
    Akao_SetGeomScreenFromState();
    otz = RotTransPers(rect, &coords[0], &coords[1], &coords[2]);
    coords[1] = (short)coords[0];
    coords[2] = coords[0] >> 16;
    Render_ResetGteScreenOffset();

    screen_x = coords[1];
    pan = (((screen_x + 0x28) << 7) / 400) + 0x40;
    *out_pan = pan;
    if ((u32)*out_pan >= 0x100) *out_pan = 0xFF;

    min_depth = D_800B0DD0;
    if (otz < min_depth) goto set_depth;
    min_depth = D_800B0DD2;
    if (!(min_depth < otz)) goto depth_done;
set_depth:
    otz = min_depth;
depth_done:;

    max_depth = *(u16 *)(game + 0xFA);
    otz = max_depth - otz;
    squared = otz * otz;
    min_depth = *(u16 *)(game + 0xF8);
    depth_range = max_depth - min_depth;
    attenuation = squared / depth_range;
    volume_delta = game[0xF7];
    min_volume = game[0xF6];
    volume_delta -= min_volume;
    volume = ((attenuation * volume_delta) / depth_range) + min_volume;
    *out_volume = volume;
    if ((u32)*out_volume >= 0x80) *out_volume = 0x7F;
    return 0;
}
