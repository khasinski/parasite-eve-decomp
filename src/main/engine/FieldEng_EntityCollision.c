#include "common.h"
typedef struct { s16 vx, vy, vz, pad; } SVECTOR;
int func_800C62DC(SVECTOR *from, void *to);
extern char *D_8009D254;

int func_800C6B20(void *arg0) {
    SVECTOR pos;
    char *entity = D_8009D254;
    int first;
    register int value asm("$2");
    SVECTOR *ptr;
    value = *(s16 *)(entity + 0x2A);
    ptr = &pos;
    pos.vx = value;
    value = *(s16 *)(entity + 0x2E);
    pos.vy = value;
    value = *(s16 *)(entity + 0x32);
    pos.vz = value;
    first = func_800C62DC(ptr, arg0);
    return first | func_800C62DC(ptr, (char *)arg0 + 8);
}


extern char *D_8009D254;

int func_800C6B90(s16 *pos, int extraRadius) {
    volatile int delta[3];
    volatile s16 local[4];
    char *entity = D_8009D254;
    int x = *(s16 *)(entity + 0x2A);
    int radius = *(s16 *)(entity + 0x224);
    int z;
    int x_sq;
    int z_sq;
    int radius_sq;
    local[0] = x;
    {
        int y;
        y = *(s16 *)(entity + 0x2E);
        local[1] = y;
    }
    z = *(s16 *)(entity + 0x32);
    local[2] = z;
    x -= pos[0];
    x_sq = x * x;
    delta[0] = x;
    z -= pos[2];
    z_sq = z * z;
    radius += extraRadius;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}


#include "common.h"
int FieldEng_GetStatus(char *obj);

extern char *D_8009D254;

int func_800C6C18(char *obj) {
    int enabled;

    if (FieldEng_GetStatus(obj) == 3) {
        enabled = (*(u8 *)(*(char **)(*(char **)(*(char **)(obj + 8) + 0) + 0x18) + 0) == 2);
    } else {
        enabled = 1;
    }

    if (enabled != 0) {
        if (FieldEng_GetStatus(obj) == 3) {
            *(int *)(*(char **)D_8009D254 + 0x4C) |= 0x4000;
            *(int *)(*(char **)(*(char **)(obj + 8) + 0) + 0) |= 0x80000000;
        }
    }

    return enabled;
}
