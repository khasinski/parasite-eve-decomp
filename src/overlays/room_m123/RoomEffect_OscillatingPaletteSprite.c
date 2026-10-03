#include "room_m123_effects.h"
#include "pe1/gte_types.h"

extern GteShortVector D_8018F1F8;
extern GteShortVector D_8018F1CC;
extern char D_80195624[];
extern int D_800E27EC, D_800F3428;
extern u16 D_800E1204[];
extern void func_800CF3AC(void *, void *, int);
extern int func_80077DC4(int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);
extern volatile u16 D_800E11E8;
extern u16 D_800E2850[];
extern volatile u16 D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern volatile u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern void func_800CE8F0(void *, int, void *, void *);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);
extern int func_800CE560(void *, int, int, int (*)(int, RoomM123Wave *));
extern RoomM123Wave *func_800CE610(void *);
extern int func_80071A54(void);

int func_80194F68(int mode, RoomM123Wave *wave) {
    int color[2];
    GteShortVector position = D_8018F1F8;
    int result, scale, kind, palette;
    u16 clut;
    switch (mode) {
    case 1:
        if (wave->state != 0) break;
        wave->frame++;
        wave->y -= 3;
        if ((s16)wave->frame >= 24) return 1;
        break;
    case 2:
        if (wave->state != 0) break;
        func_800CF3AC(D_80195624, color, (s16)wave->frame);
        result = func_80077DC4((D_800E27EC << 10) / 24);
        position.z = D_800E27EC << 5;
        scale = result / 2 + 2048;
        kind = *(u16 *)&D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        func_800CEE20(wave, &position, scale, scale,
                      *(s16 *)&D_800F336A + 220, clut, 1, 128, color);
        break;
    }
    return 0;
}

typedef struct RoomM123Burst {
    u16 frame;
    u16 phase;
    u16 x, y, z;
    u16 pad;
} RoomM123Burst;

int func_80195114(int mode, RoomM123Burst *burst, int *choice) {
    GteShortVector position = D_8018F1CC;
    RoomM123Wave *child;
    int value;

    switch (mode) {
    case 0:
        value = *choice ? 34 : 40;
        func_800CE8F0(D_800F32D0->pool, value, &position, &burst->x);
        burst->frame = 0;
        burst->phase = 0;
        func_800D3F64(0x581, func_800D3FD8());
        return func_800CE560(D_800F33E0->pool, 12, 24, func_80194F68);
    case 1:
        if (D_800E27EC < 32) {
            child = func_800CE610(D_800F33E0->pool);
            if (child != 0) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                {
                    int sample = func_80071A54();
                    int previous = child->x;
                    child->x = (previous - 128) + (sample & 255);
                }
                {
                    int sample = func_80071A54();
                    int previous = child->y;
                    child->y = (previous - 128) + (sample & 255);
                }
                {
                    int sample = func_80071A54();
                    int previous = child->z;
                    child->z = (previous - 128) + (sample & 255);
                }
                child->state = 0;
                child->frame = 0;
            }
        }
        if (D_800E27EC >= 8) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11E8;
        int palette;
        D_800F3368 = 16;
        D_800F336A = 1;
        D_800F3376 = 16;
        D_800F3378 = 16;
        palette = D_800E2850[paletteIndex];
        D_800F336C = 2;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 0;
        D_800F3370 = palette;
        break;
    }
    }
    return 0;
}
