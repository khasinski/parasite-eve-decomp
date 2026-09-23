#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM123Wave {
    u16 x;
    u16 y;
    u16 z;
    u16 pad;
    s16 state;
    u16 frame;
} RoomM123Wave;

extern GteShortVector D_8018F1F8;
extern char D_80195624[];
extern int D_800E27EC, D_800F3428;
extern s16 D_800F336A;
extern u16 D_800F336C, D_800E1204[];
extern void func_800CF3AC(void *, void *, int);
extern int func_80077DC4(int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

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
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        func_800CEE20(wave, &position, scale, scale, D_800F336A + 220, clut, 1, 128, color);
        break;
    }
    return 0;
}
