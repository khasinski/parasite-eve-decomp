#include "common.h"

typedef struct RoomTransformScaleOwner {
    char pad0[8];
    char *actor;
    void (*callback)(struct RoomTransformScaleOwner *);
    char pad10[0x22];
    s16 timer;
    s16 duration;
} RoomTransformScaleOwner;

extern s32 D_800966EC[];
extern s32 Scratch_1F800008 asm("0x1F800008");
extern s32 Scratch_1F80000C asm("0x1F80000C");
extern s32 Scratch_1F800010 asm("0x1F800010");

void RotMatrixYXZ(void *rotation, void *matrix);
void Gte_ScaleMatrix(void *matrix, s32 *scale);

int ROOMLIB_UPDATE_TRANSFORM_SCALE_NAME(RoomTransformScaleOwner *owner) {
    char *actor;
    s32 *matrix;
    s32 *scale;
    s32 scaleValue;
    s32 curve;

    actor = owner->actor;
    scale = (s32 *)0x1F800008;
    owner->callback(owner);

    matrix = (s32 *)(actor + 0x1E8);
    matrix[5] = *(s16 *)(actor + 0x2A);
    matrix[6] = *(s16 *)(actor + 0x2E);
    matrix[7] = *(s16 *)(actor + 0x32);
    RotMatrixYXZ(actor + 0x38, matrix);

    scaleValue = *(u16 *)(actor + 0x26);
    Scratch_1F800010 = scaleValue;
    Scratch_1F80000C = scaleValue;
    Scratch_1F800008 = scaleValue;
    Gte_ScaleMatrix(matrix, (s32 *)0x1F800008);

    if (owner->timer != 0) {
        curve = (D_800966EC[(((owner->duration - owner->timer) << 11) /
                              owner->duration) & 0xFFF] << 16) >> 17;
        Scratch_1F800010 = 0x1000 + curve;
        scale[0] = 0x1000 + curve;
        Scratch_1F80000C = 0x1000 - curve;
        Gte_ScaleMatrix(matrix, scale);
        owner->timer--;
    }

    return 0;
}

#undef ROOMLIB_UPDATE_TRANSFORM_SCALE_NAME
