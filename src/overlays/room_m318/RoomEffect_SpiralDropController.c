#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Spiral drop: attaches to the actor named by its parameters; for 51
 * frames it drops a falling spark from a point circling the actor on a
 * turning angle, while a glowing ring pulses there and sets the radius. */
int func_80192ED4(int mode, RoomSpiralDrop *drop, RoomEmberBurstParams *params) {
    RenderColor color;
    GteShortVector position;
    GteRotation spin;
    RoomSpiralDropSpark *spark;
    int size;

    switch (mode) {
    case 0:
        drop->angle = func_80071A54();
        drop->radius = 0;
        RoomEffect_AttachToActor(params->subId, params->typeId, drop);
        return func_800CE560(D_800F33E0->pool, 8, 0x18, func_80192DA0);
    case 1:
        if (D_800E27EC < 0x33) {
            spark = (RoomSpiralDropSpark *)func_800CE610(D_800F33E0->pool);
            if (spark) {
                spark->x = drop->x + func_80077DC4(drop->angle) * drop->radius / 4096;
                spark->z = drop->z + func_80077CF4(drop->angle) * drop->radius / 4096;
                spark->y = drop->y;
                spark->speed = func_80071A54() & 3;
                drop->angle += 0x8AA + (func_80071A54() & 0x1F);
            }
        }
        if (D_800E27EC < 0x4A) break;
        return 1;
    case 2:
        D_800F3368.depth = 8;
        if (D_800E27EC < 0x33) {
            position.x = drop->x;
            position.y = drop->y;
            position.z = drop->z;
            spin.x = 0x400;
            spin.y = 0;
            spin.z = D_800E27EC << 5;
            spin.flags = 1;
            size = func_80077CF4((D_800E27EC << 10) / 50);
            drop->radius = size / 8;
            func_800CF3AC(D_801994BC, &color, D_800E27EC);
            func_800D0728(&position, 350, 500, 0x14, &spin, size, size, 0, &color,
                          0x80, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
