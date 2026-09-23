#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM318Orbit {
    u16 x, y, z, pad;
    int angle;
} RoomM318Orbit;

typedef struct RoomM318Node {
    char pad[8];
    void *link;
} RoomM318Node;

extern int D_801995B8, D_800E27EC;
extern char D_801995BC[], D_801995DC[];
extern RoomM318Node *D_800F32D0;
extern u16 D_800F3374;
extern void func_800CE870(void *, int, void *);
extern int func_80077DC4(int), func_80077CF4(int);
extern void func_800CF3AC(void *, void *, int);
extern void func_800D004C(void *, int, int, int, void *, int, int, void *, void *, int, int);
extern void func_800D0728(void *, int, int, int, void *, int, int, int, void *, int, int);

int func_80193D88(int mode, RoomM318Orbit *orbit) {
    GteShortVector position;
    int color[2];
    int scale, sine, cosine, product;
    register int result asm("$2");
    register int delta asm("$4");
    void *palette;

    /* Reuse the mode temporary to preserve the retail arithmetic registers. */
    product = mode;
    switch (product) {
    case 0:
        orbit->angle = D_801995B8;
        D_801995B8 += 0x555;
        func_800CE870(D_800F32D0->link, 0, orbit);
        orbit->y -= 100;
        sine = func_80077DC4(orbit->angle);
        orbit->x += sine * 300 / 4096;
        cosine = func_80077CF4(orbit->angle);
        product = cosine * 300;
        asm volatile("" : : "r"(product));
        result = 0;
        delta = product / 4096;
        orbit->z += delta;
        return result;
    case 1:
        if (D_800E27EC >= 8) return 1;
        break;
    case 2:
        palette = D_801995BC;
        D_800F3374 = 60;
        asm volatile("" ::: "memory");
        position.x = orbit->x;
        position.y = orbit->y;
        position.z = orbit->z;
        scale = (D_800E27EC << 9) + 2048;
        func_800CF3AC(palette, color, D_800E27EC);
        func_800D004C(&position, 500, 500, 16, 0, scale, scale, color, 0, 128, 1);
        func_800D0728(&position, 410, 500, 20, 0, 4096, 4096, 0, color, 128, 3);
        func_800CF3AC(D_801995DC, color, D_800E27EC);
        func_800D004C(&position, 500, 100, 16, 0, scale, scale, color, 0, 128, 1);
        break;
    }
    return 0;
}
