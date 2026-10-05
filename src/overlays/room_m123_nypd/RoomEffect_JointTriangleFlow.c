#include "pe1/gte.h"
#include "pe1/room_m123.h"
#include "pe1/room_m123_joint_triangle.h"
#include "room_m123_effects.h"

int func_8019251C(int mode, RoomM123JointTriangle *triangle) {
    GteShortVector corners[4];
    GteShortVector offset = D_8018F1CC;
    GteShortVector tip = D_8018F1D4;
    RenderColor color = D_8018F1DC;
    int weight;
    int width;
    int position;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        switch (triangle->state) {
        case 0:
            triangle->frame++;
            if (triangle->frame >= 24) return 1;
            break;
        case 1:
            triangle->frame++;
            triangle->position = (s16)(triangle->position + 0x200) % 0x3000;
            if (triangle->frame >= 24) return 1;
            break;
        default:
            return 0;
        }
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        FieldEng_TransformMatrixPoint(D_800F32D0->pool, 0x28, &offset, &corners[0]);
        FieldEng_TransformMatrixPoint(D_800F32D0->pool, 0x22, &offset, &corners[1]);
        FieldEng_TransformMatrixPoint(D_800F32D0->pool, 0x1D, &tip, &corners[2]);
        switch (triangle->state) {
        case 0:
            FieldEng_TransformMatrixPoint(D_800F32D0->pool, 0x17, &tip, &corners[3]);
            weight = 0x5000 - rsin((triangle->frame << 10) / 24) * 4;
            LoadAverageShort12(&corners[3], &corners[0], 0x1000 - weight, weight, &corners[0]);
            LoadAverageShort12(&corners[3], &corners[1], 0x1000 - weight, weight, &corners[1]);
            LoadAverageShort12(&corners[3], &corners[2], 0x1000 - weight, weight, &corners[2]);
            width = (triangle->frame << 7) / 24;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800D2B58(&corners[0], &corners[1], &color, &color, width, width, 1);
            func_800D2B58(&corners[1], &corners[2], &color, &color, width, width, 1);
            func_800D2B58(&corners[2], &corners[0], &color, &color, width, width, 1);
            return 0;
        case 1:
            position = triangle->position;
            if (position < 0x1000) {
                weight = position;
                LoadAverageShort12(&corners[0], &corners[1], 0x1000 - weight, weight, &corners[0]);
            } else if (position < 0x2000) {
                weight = position - 0x1000;
                LoadAverageShort12(&corners[1], &corners[2], 0x1000 - weight, weight, &corners[0]);
            } else {
                weight = position - 0x2000;
                LoadAverageShort12(&corners[2], &corners[0], 0x1000 - weight, weight, &corners[0]);
            }
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            width = rsin(triangle->frame << 8) / 32;
            func_800D1DEC(&corners[0], &color, width, 1);
            break;
        default:
            return 0;
        }
        break;
    }
    return 0;
}

/* Preserve this controller's direct halfword accesses alongside the
 * callback's structured RenderEffectParameters view. */
#define D_800F3368 (*(u16 *)0x800F3368)
#define D_800F336A (*(u16 *)0x800F336A)
#define D_800F336C (*(u16 *)0x800F336C)
#define D_800F336E (*(u16 *)0x800F336E)
#define D_800F3370 (*(u16 *)0x800F3370)
#define D_800F3372 (*(u16 *)0x800F3372)
#define D_800F3374 (*(u16 *)0x800F3374)
#define D_800F3376 (*(u16 *)0x800F3376)
#define D_800F3378 (*(u16 *)0x800F3378)

extern int D_800E27EC;
extern u16 D_800E11EA;
extern u16 D_800E2850[];
extern int func_800CE560(void *, int, int, int (*)(void));
extern RoomM123JointTriangleEmitterView *func_800CE610(void *);

int func_801929FC(int mode) {
    int frame, index;
    RoomM123JointTriangleEmitterView *position;

    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 8, 16, func_8019251C);
    case 1:
        frame = D_800E27EC;
        if (frame == (frame / 6) * 6) {
            if (frame >= 40) return 2;
            for (index = 0; index < 3; index++) {
                position = func_800CE610(D_800F33E0->pool);
                if (position != 0) {
                    position->x = 1;
                    position->y = 0;
                    position->z = index << 12;
                }
            }
            if (D_800E27EC < 25) {
                position = func_800CE610(D_800F33E0->pool);
                if (position != 0) {
                    position->x = 0;
                    position->y = 0;
                    position->z = 0;
                }
            }
        }
        if (D_800E27EC >= 40) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11EA;
        int palette;
        D_800F3368 = 16;
        D_800F336A = 1;
        D_800F3376 = 16;
        D_800F3378 = 16;
        palette = D_800E2850[paletteIndex];
        asm volatile("" ::: "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 12;
        D_800F3370 = palette;
        break;
    }
    }
    return 0;
}
