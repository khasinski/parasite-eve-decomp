#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/field_actor.h"

int func_800C62DC(GteShortVector *from, GteShortVector *to);
extern char *D_8009D254;

int func_800C6B20(void *arg0) {
    GteShortVector pos;
    FieldActor *entity = (FieldActor *)D_8009D254;
    int first;
    register int value asm("$2");
    GteShortVector *ptr;
    value = Pe1Fixed_Integer(&entity->pos_x);
    ptr = &pos;
    pos.x = value;
    value = Pe1Fixed_Integer(&entity->pos_y);
    pos.y = value;
    value = Pe1Fixed_Integer(&entity->pos_z);
    pos.z = value;
    first = func_800C62DC(ptr, arg0);
    return first | func_800C62DC(ptr, (char *)arg0 + 8);
}

extern char *D_8009D254;

int func_800C6B90(s16 *pos, int extraRadius) {
    int delta[3];
    s16 local[4];
    char *entity = D_8009D254;
    int x = *(s16 *)(entity + 0x2A);
    int radius = (s16)((FieldActor *)entity)->render_object.hit_cylinder.radius;
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
int FieldEng_GetStatus(void *obj);

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
