#include "pe1/akao/pos.h"

typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;

extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
#define GAME ((u8 *)&D_800B0CD8_o)

extern s16 D_800B0DD0;
extern s16 D_800B0DD2;
extern int *D_800BCFA4;
extern int *D_800BCFA8;

int Render_SetGteScreenOffset(void);
int Render_ResetGteScreenOffset(void);
int RotTransPers(void *v, void *sxy, void *p, int *flag);

static void Akao_LoadRotMatrix(int *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];
    w5 = matrix[5];
    w6 = matrix[6];
    w7 = matrix[7];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Akao_SetGeomScreenFromState(void) {
    int geom_screen;

    geom_screen = *D_800BCFA8;
    asm volatile("ctc2 %0,$26" : : "r"(geom_screen));
}

static s32 Akao_ClampPan(s32 pan) {
    if ((unsigned int)pan >= 0x100) {
        return 0xFF;
    }
    return pan;
}

static s32 Akao_ClampVolume(s32 volume) {
    if ((unsigned int)volume >= 0x80) {
        return 0x7F;
    }
    return volume;
}

s32 Akao_Calc3DPan(AkaoPackedRect3 *rect, s32 *out_pan, s32 *out_volume) {
    s32 sxy;
    s32 p;
    s32 flag;
    s32 otz;
    s32 screen_x;
    s32 pan;
    s32 min_depth;
    s32 max_depth;
    s32 depth_range;
    s32 depth_delta;
    s32 attenuation;
    s32 min_volume;
    s32 volume_delta;
    s32 volume;

    Render_SetGteScreenOffset();
    Akao_LoadRotMatrix(D_800BCFA4);
    Akao_SetGeomScreenFromState();
    otz = RotTransPers(rect, &sxy, &p, &flag);
    Render_ResetGteScreenOffset();

    screen_x = (short)sxy;
    pan = (((screen_x + 0x28) << 7) / 400) + 0x40;
    *out_pan = Akao_ClampPan(pan);

    min_depth = D_800B0DD0;
    max_depth = D_800B0DD2;
    if (otz < min_depth) {
        otz = min_depth;
    } else if (max_depth < otz) {
        otz = max_depth;
    }

    max_depth = *(u16 *)(GAME + 0xFA);
    min_depth = *(u16 *)(GAME + 0xF8);
    depth_range = max_depth - min_depth;
    if (depth_range == 0) {
        *out_volume = Akao_ClampVolume(GAME[0xF6]);
        return 0;
    }

    depth_delta = max_depth - otz;
    attenuation = (depth_delta * depth_delta) / depth_range;
    min_volume = GAME[0xF6];
    volume_delta = GAME[0xF7] - min_volume;
    volume = ((attenuation * volume_delta) / depth_range) + min_volume;
    *out_volume = Akao_ClampVolume(volume);
    return 0;
}
