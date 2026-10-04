#include "pe1/scene_e22_ember_burst.h"

/* Link beam controller: strings two light ribbons across three actor
 * joints; for 33 frames the last joint sheds a glow spark along its
 * heading, after which the ribbons fade out. */
int func_80193414(int mode, SceneE22LinkBeam *beam) {
    GteShortVector first;
    GteShortVector middle;
    GteShortVector last;
    GteShortVector heading;
    GteShortVector offset0 = D_8018F1D8;
    GteShortVector offset1 = D_8018F1E0;
    RenderColor color0 = D_8018F1E8;
    RenderColor color1 = D_8018F1EC;
    RenderColor color2 = D_8018F1F0;
    RoomOrbitTrailParticle *spark;

    switch (mode) {
    case 0:
        beam->alpha = 0x80;
        beam->reserved00 = 0;
        beam->timer = 0;
        beam->jointSet = D_800E2368->jointSet;
        func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].first,
                      &offset0, &first);
        func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].middle,
                      &offset0, &middle);
        func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].last,
                      &offset1, &last);
        func_800D1384(&first, &middle, 0x3F2, &color2, &color1, 0x80,
                      beam->trailA, 1);
        func_800D1384(&middle, &last, 0x3F2, &color1, &color0, 0x80,
                      beam->trailB, 1);
        return func_800CE560(D_800F33E0->pool, 0x14, 8, func_801931B8);
    case 1:
        beam->timer++;
        func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].last,
                      &offset1, &first);
        func_800CE9D4(D_800F32D0->pool, D_801991B8[beam->jointSet].last,
                      &heading);
        if (beam->timer < 0x21) {
            spark = func_800CE610(D_800F33E0->pool);
            if (spark != 0) {
                spark->x = first.x;
                spark->y = first.y;
                spark->z = first.z;
                func_800CFB7C(&heading, 0x32, &spark->heading);
                spark->state = 0;
                spark->timer = 0;
            }
        } else if (beam->alpha > 0) {
            beam->alpha -= 0x10;
        }
        if (D_800E27EC == 4)
            func_800D3F64(0x5D3, func_800D3FD8());
        if (beam->timer < 0x20) break;
        return 2;
    case 2:
        if (beam->alpha >= 0) {
            func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].first,
                          &offset0, &first);
            func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].middle,
                          &offset0, &middle);
            func_800CE8F0(D_800F32D0->pool, D_801991B8[beam->jointSet].last,
                          &offset1, &last);
            func_800D1384(&first, &middle, 10, &color2, &color1, beam->alpha,
                          beam->trailA, 1);
            func_800D1384(&middle, &last, 10, &color1, &color0, beam->alpha,
                          beam->trailB, 1);
        }
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x10;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
