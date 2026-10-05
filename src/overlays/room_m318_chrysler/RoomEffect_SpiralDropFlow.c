#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

extern char D_8019948C[];

int func_80192DA0(int mode, volatile RoomSpiralDropSparkRecord *record) {
    volatile RoomSpiralDropSparkCallbackView *particle = &record->callback;
    int color[2];
    GteShortVector position;
    switch (mode) {
    case 1: {
        int speed = particle->speed - 1;
        register int y asm("$2") = particle->y;
        register int delta asm("$5") = particle->speed;
        int frame = D_800E27EC;
        /* Preserve the retail read order and register lifetimes. */
        asm("" : : "r"(delta), "r"(frame), "r"(speed), "r"(y));
        ((RoomSpiralDropSparkCallbackView *)particle)->y = y - delta;
        ((RoomSpiralDropSparkCallbackView *)particle)->speed = speed;
        if (frame >= 24) return 1;
        break;
    }
    case 2: {
        int palette;
        int kind;
        int z;
        unsigned short clut;
        func_800CF3AC(D_8019948C, color, D_800E27EC);
        kind = D_800F336C;
        position.x = particle->x;
        position.y = particle->y;
        z = particle->z;
        position.z = z;
        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 10 : palette + 6);
        func_800CEE20((GteShortVector *)&position, 0, 2048, 2048, 6, clut, 1, 128, color);
        break;
    }
    }
    return 0;
}

/* Spiral drop: attaches to the actor named by its parameters; for 51
 * frames it drops a falling spark from a point circling the actor on a
 * turning angle, while a glowing ring pulses there and sets the radius. */
int func_80192ED4(int mode, RoomSpiralDrop *drop, RoomEmberBurstParams *params) {
    RenderColor color;
    GteShortVector position;
    GteRotation spin;
    RoomSpiralDropSparkRecord *spark;
    int size;

    switch (mode) {
    case 0:
        drop->angle = func_80071A54();
        drop->radius = 0;
        RoomEffect_AttachToActor(params->subId, params->typeId, drop);
        return func_800CE560(D_800F33E0->pool, 8, 0x18, func_80192DA0);
    case 1:
        if (D_800E27EC < 0x33) {
            spark = (RoomSpiralDropSparkRecord *)func_800CE610(D_800F33E0->pool);
            if (spark) {
                spark->emitter.x = drop->x + func_80077DC4(drop->angle) * drop->radius / 4096;
                spark->emitter.z = drop->z + func_80077CF4(drop->angle) * drop->radius / 4096;
                spark->emitter.y = drop->y;
                spark->emitter.speed = func_80071A54() & 3;
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
