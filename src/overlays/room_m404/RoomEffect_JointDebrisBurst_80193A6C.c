#include "pe1/room_m404_effects.h"

/* Bursts six bouncing sparks and two falling ones from a model joint
 * during the first frames of the effect. */
int func_80193A6C(int mode, RoomM404BurstOrigin *origin) {
    RoomM404JointTemplate template = D_8018F21C;
    RoomM404BurstSpark *spark;
    int i;

    switch (mode) {
    case 0:
        func_800CE870(D_800F32D0->pool, 0, (s16 *)origin);
        func_800CE8F0(D_800F32D0->pool, 0, &template, origin);
        func_800D3F64(0x5C0, func_800D3FD8());
        func_800D3F64(0x5C1, 0x80);
        return func_800CE560(D_800F33E0->pool, 20, 32, func_801935E0);
    case 1:
        if (D_800E27EC < 5) {
            for (i = 0; i < 6; i++) {
                spark = func_800CE610(D_800F33E0->pool);
                if (spark != 0) {
                    spark->x = origin->x;
                    spark->y = origin->y;
                    spark->z = origin->z;
                    spark->vx = func_80071A54() % 50 - 25;
                    spark->vy = -(func_80071A54() % 39);
                    spark->vz = func_80071A54() % 50 - 25;
                    spark->state = 1;
                    spark->timer = 0;
                }
            }
            for (i = 0; i < 2; i++) {
                spark = func_800CE610(D_800F33E0->pool);
                if (spark != 0) {
                    spark->x = origin->x;
                    spark->y = origin->y;
                    spark->z = origin->z;
                    spark->vx = func_80071A54() % 80 - 40;
                    spark->vy = -(func_80071A54() % 80);
                    spark->vz = func_80071A54() % 80 - 40;
                    spark->state = 0;
                    spark->timer = 0;
                }
            }
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2:
        D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}
