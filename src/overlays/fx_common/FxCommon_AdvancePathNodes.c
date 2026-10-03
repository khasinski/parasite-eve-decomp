#include "fx_common_motion.h"

/* Move the path-following nodes one step along their recorded paths, bank
 * the two flyers into their turns, and copy each flyer onto its shadow. */
void func_80193478(void)
{
    GteShortVector bank0;
    GteShortVector bank1;

    func_8018F55C(D_8019C038, 0x4A, func_8006EC6C(&D_801D0260, 2),
                  (FxCommonMotionVec *)g_FxCommonPathNodes[4]->matrix.t,
                  &g_FxCommonPathNodes[4]->seed);
    func_8018F55C(D_8019C038, 0x4B, func_8006EC6C(&D_801D0260, 2),
                  (FxCommonMotionVec *)g_FxCommonPathNodes[5]->matrix.t,
                  &g_FxCommonPathNodes[5]->seed);
    func_8018F55C(D_8019C03C, 0x4C, func_8006EC6C(&D_801D0260, 2),
                  (FxCommonMotionVec *)g_FxCommonPathNodes[0]->matrix.t, &bank0);
    func_8018F55C(D_8019C03C, 0x4D, func_8006EC6C(&D_801D0260, 2),
                  (FxCommonMotionVec *)g_FxCommonPathNodes[2]->matrix.t, &bank1);

    g_FxCommonPathNodes[0]->seed.component[0] = bank0.x;
    g_FxCommonPathNodes[0]->seed.component[1] = bank0.y;
    g_FxCommonPathNodes[0]->seed.component[2] += bank0.z;
    if (g_FxCommonPathNodes[0]->seed.component[2] > 0x200)
        g_FxCommonPathNodes[0]->seed.component[2] = 0x200;
    if (g_FxCommonPathNodes[0]->seed.component[2] < -0x200)
        g_FxCommonPathNodes[0]->seed.component[2] = -0x200;
    if (g_FxCommonPathNodes[0]->seed.component[2] > 0)
        g_FxCommonPathNodes[0]->seed.component[2] -= 8;
    if (g_FxCommonPathNodes[0]->seed.component[2] < 0)
        g_FxCommonPathNodes[0]->seed.component[2] += 8;

    g_FxCommonPathNodes[2]->seed.component[0] = bank1.x;
    g_FxCommonPathNodes[2]->seed.component[1] = bank1.y;
    g_FxCommonPathNodes[2]->seed.component[2] += bank1.z;
    if (g_FxCommonPathNodes[2]->seed.component[2] > 0x180)
        g_FxCommonPathNodes[2]->seed.component[2] = 0x180;
    if (g_FxCommonPathNodes[2]->seed.component[2] < -0x180)
        g_FxCommonPathNodes[2]->seed.component[2] = -0x180;
    if (g_FxCommonPathNodes[2]->seed.component[2] > 0)
        g_FxCommonPathNodes[0]->seed.component[2] -= 8;
    if (g_FxCommonPathNodes[2]->seed.component[2] < 0)
        g_FxCommonPathNodes[0]->seed.component[2] += 8;

    g_FxCommonPathNodes[4]->seed.component[1] -= 0x400;
    g_FxCommonPathNodes[5]->seed.component[1] -= 0x400;
    g_FxCommonPathNodes[0]->seed.component[1] -= 0x400;
    g_FxCommonPathNodes[2]->seed.component[1] -= 0x400;

    g_FxCommonPathNodes[1]->matrix = g_FxCommonPathNodes[0]->matrix;
    g_FxCommonPathNodes[1]->seed = g_FxCommonPathNodes[0]->seed;
    g_FxCommonPathNodes[3]->matrix = g_FxCommonPathNodes[2]->matrix;
    g_FxCommonPathNodes[3]->seed = g_FxCommonPathNodes[2]->seed;
    D_8019C038 += 4;
    D_8019C03C += 0x10;
}
