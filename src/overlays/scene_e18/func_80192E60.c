/* CC1_FLAGS: -fno-cse-skip-blocks */
#include "common.h"

typedef struct Effect {
    s16 field00;
    s16 field02;
    s16 field04;
    s16 field06;
    s16 frame;
    s16 speed;
    s16 size;
} Effect;

typedef struct SceneE18Object {
    char reserved[0x1FC];
    s32 x, y, z;
} SceneE18Object;

typedef struct SceneE18ObjectLink {
    char reserved[8];
    SceneE18Object *object;
} SceneE18ObjectLink;

extern SceneE18ObjectLink *D_800F32D0;
extern int D_800E27EC;
extern u16 D_801941CC;
void func_800D0728(void *position, int start, int end, int count,
                   void *params, int frameA, int frameB, void *partA,
                   void *partB, int size, int active);

int func_80192E60(int mode, Effect *effect) {
    s16 position[4];
    s16 params[4];
    SceneE18Object *object;

    if (mode == 1) {
        effect->frame += effect->speed;
        effect->speed += 0x30;
        effect->size -= 0x20;
        if (effect->size < 0x20) {
            D_801941CC--;
            return 1;
        }
        return 0;
    }
    if (mode != 2) {
        return 0;
    }

    object = D_800F32D0->object;
    position[0] = object->x;
    position[1] = object->y;
    position[2] = object->z;
    params[0] = 0x400;
    params[1] = D_800E27EC << 7;
    params[2] = 0;
    params[3] = 1;
    func_800D0728(position, 0x300, 0x400, 0x10, params,
                   effect->frame, effect->frame, effect, &effect->field04,
                   effect->size > 0x80 ? 0x80 : effect->size, 1);
    func_800D0728(position, 0x400, 0x500, 0x10, params,
                   effect->frame, effect->frame, &effect->field04, effect,
                   effect->size > 0x80 ? 0x80 : effect->size, 1);
    return 0;
}
