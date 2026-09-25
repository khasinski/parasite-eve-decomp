#include "common.h"
#include "scene_particle_slots.h"

typedef struct Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Matrix;

typedef struct Vec4 {
    s32 x, y, z, w;
} Vec4;

typedef struct SceneParticleOwner {
    u8 reserved[0x24];
    u8 drawMode;
} SceneParticleOwner;

typedef struct SceneFxColor {
    u8 r, g, b, pad3;
    u8 t4, t5, t6, pad7;
    s16 h8, hA;
} SceneFxColor;

extern s16 D_801994B2;
void *func_800C2B50(void);
void func_80071A44(Vec4 *, int, int);
void func_80078CC4(Matrix *, Vec4 *);
void func_800C2EAC(u8);
void func_800C2FF0(int, int);
void func_800C3098(int);
void func_800C3238(int);
void func_800C42A4(void *, Matrix *, int);

void func_8018FBA4(void *unused0, void *unused1, SceneParticleCluster *argSlots) {
    SceneParticleCluster *slots = argSlots;
    Matrix matrix;
    Vec4 scratch, draw;
    SceneParticleOwner *owner;
    register unsigned i asm("$19");
    register char *scale asm("$17");
    register char *position asm("$18");
    s16 *slot = &D_801994B2;

    owner = func_800C2B50();
    i = 0;
    /* Hoist the draw state address before the cursor setup. */
    asm("" : : "r"(slot));
    scale = (char *)slots;
    /* Keep the scale cursor live before the first draw-state call. */
    asm("" : "=r"(scale) : "0"(scale));
    func_800C2EAC(owner->drawMode);
    position = scale;
    func_800C3098(0x10);
    func_800C3238(2);

    for (; i < 30; position += 8, i++, scale += 2) {
        if (slots->active[i] == 1) {
            matrix.m[2][2] = 0x1000;
            matrix.m[1][1] = 0x1000;
            matrix.m[0][0] = 0x1000;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;

            func_80071A44(&draw, 0, 0x10);
            draw.x = ((SceneParticleCluster *)scale)->fieldF0[0];
            draw.y = ((SceneParticleCluster *)scale)->fieldF0[0];
            draw.z = ((SceneParticleCluster *)scale)->fieldF0[0];
            scratch = draw;
            func_80078CC4(&matrix, &scratch);

            matrix.t[0] = ((SceneParticleOffset *)position)->x;
            matrix.t[1] = ((SceneParticleOffset *)position)->y;
            matrix.t[2] = ((SceneParticleOffset *)position)->z;
            *slot = ((SceneParticleCluster *)scale)->brightness[0];
            if (slots->texture[i] == 0x44) {
                func_800C2FF0(0x20, 0x20);
                ((SceneFxColor *)((char *)slot - 10))->t4 = slots->texture[i];
            }
            if (slots->texture[i] == 0x4C) {
                func_800C2FF0(0x10, 0x10);
                {
                    u8 color = slots->texture[i];
                    u8 counter = slots->counter[i];
                    color += 3;
                    ((SceneFxColor *)((char *)slot - 10))->t4 = color - (counter >> 1);
                }
            }
            if (slots->texture[i] == 0x58) {
                func_800C2FF0(0x10, 0x10);
                {
                    u8 color = slots->texture[i];
                    u8 counter = slots->counter[i];
                    color += 7;
                    ((SceneFxColor *)((char *)slot - 10))->t4 = color - (counter >> 1);
                }
            }
            ((SceneFxColor *)((char *)slot - 10))->t5 = slots->mode[i];
            func_800C42A4((char *)slot - 10, &matrix, 1);
        }
    }
}
