#include "common.h"
#include "pe1/field_anim.h"

int rand(void);

void FieldAnim_InitRandomOffsetPairs(void *arg0, void *arg1,
                                     FieldAnimBurstData *anim) {
    FieldAnimPointTriple *base;
    u16 *base_values;
    FieldAnimBurstWindow *window;
    u8 *entry;
    u16 z;
    int i;

    base = &D_800E27F8;
    i = 0;

    *(u16 *)anim->header.origin_x = base->x;
    *(u16 *)anim->header.origin_y = base->y;
    z = base->z;
    *(u16 *)anim->header.scale = 0x7F;
    *(u16 *)anim->header.duration = 0x3B4;
    anim->header.mode = 0;
    *(u16 *)anim->header.origin_z = z;
    base_values = &base->x;

    do {
        entry = &anim->bytes[(i & 0xFFFF) * 8];
        window = (FieldAnimBurstWindow *)entry;
        window->point.x = base_values[0];
        window->point.y = base_values[1];
        window->point.z = base_values[2];
        window->offset.x = (rand() % 50) - 0x19;
        window->offset.y = (rand() % 50) - 0x19;
        i++;
        window->offset.z = 0;
    } while (((unsigned int)i & 0xFFFF) < 2);
}
