#include "pe1/room_m256_effects.h"

/* Swirl spark controller: plays its two sounds on start, then while the
 * effect is young sheds two swirl sparks per frame from the tracked joint,
 * aimed through one of three (then one of two) headings picked by a
 * rotating counter; mode 2 sets the sprite parameters. */
int func_80193BFC(int mode, s16 *counter) {
    RoomM256Template template = D_8018F210;
    GteShortVector position;
    GteShortVector base;
    GteShortVector angles;
    RoomM256SwirlSpark *child;
    void **soundSlot;
    int distance = 0;
    int volume;
    int time;

    switch (mode) {
    case 0:
        *counter = 0;
        soundSlot = &D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            func_8006DF50(*soundSlot, 0x5A7, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5BF, 0x80, 0x80, volume);
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_8019377C);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 4, &template, &position);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 4, &base);
        if (D_800E27EC < 0x6F) {
            child = (RoomM256SwirlSpark *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter) {
                case 0:
                    distance = (func_80071A54() & 7) + 14;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (func_80071A54() & 0x7F) - 0x40;
                    break;
                case 1:
                    distance = (func_80071A54() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (func_80071A54() & 0x7F) + 0x3C0;
                    break;
                case 2:
                    distance = (func_80071A54() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (func_80071A54() & 0x7F) + 0x3C0;
                    break;
                }
                func_800CFB7C(&angles, distance, (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
            child = (RoomM256SwirlSpark *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter & 1) {
                case 0:
                    distance = (func_80071A54() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (func_80071A54() & 0xFF) + 0x380;
                    break;
                case 1:
                    distance = (func_80071A54() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (func_80071A54() & 0xFF) + 0x380;
                    break;
                }
                func_800CFB7C(&angles, distance, (GteShortVector *)&child->vx);
                child->small = func_80071A54() & 1;
                child->state = 1;
                child->timer = 0;
            }
            *counter = (s16)(*counter + 1) % 3;
        }
        if (D_800E27EC < 2) {
            break;
        }
        return 2;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}
