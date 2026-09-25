#include "common.h"

typedef struct SceneE18WaveEffect {
    s16 x, y, z, field06;
    s16 field08, field0a, field0c, field0e;
    s16 size, wave, amplitude;
} SceneE18WaveEffect;
extern s32 D_800E27EC;
extern u16 D_800F336C;
extern u16 D_800E1204[];
extern s32 D_800F3428;
extern void *D_800F32D0;
extern u32 D_80194180[], D_80194184[];
int func_80077CF4(int);
int func_80077AA4(int,int);
void func_800CEE20(void *, void *, int, int, int, int, int, int, int);
void func_800D004C(void *, int, int, int, void *, int, int, void *, void *, int, int);

int func_80193A58(int mode, SceneE18WaveEffect *effect) {
    s16 position[4];
    s16 params[4];
    int palette;
    register u16 index;
    /* Preserve the load-delay slots in the original mode-2 sequence. */
    register int offset asm("$3");
    register int specialIndex asm("$4");
    if (mode == 1) {
        effect->wave = (func_80077CF4(D_800E27EC << 4) * effect->amplitude) >> 12;
        if (D_800E27EC < 8) return 0;
        effect->size -= 8;
        if (effect->size >= 16) goto ret0;
        return 1;
    }
    if (mode != 2) return 0;
    specialIndex = 4;
    index = D_800F336C;
    offset = index << 1;
    palette = *(u16 *)((char *)D_800E1204 + offset);
    if (index == specialIndex && D_800F3428 != 0) palette += 7;
    else palette += 3;
    palette = func_80077AA4(0, palette);
    func_800CEE20(effect, &effect->field08, effect->wave, effect->wave,
                  0x40, palette & 0xffff, 3, effect->size, 0);
    position[0] = effect->x;
    position[1] = *(int *)(*(char **)((char *)D_800F32D0 + 8) + 0x200);
    position[2] = effect->z;
    params[0] = 0x400;
    params[1] = (D_800E27EC * 3) << 6;
    params[2] = 0;
    params[3] = 1;
    func_800D004C(position, 0x100, 0x100, 10, params,
                  effect->wave, effect->wave, D_80194180, D_80194184,
                  effect->size, 1);
ret0:
    return 0;
}
