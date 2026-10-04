#include "pe1/room_glint_ribbon.h"
#ifndef ROOMEFFECT_GLINT_RIBBON_FUNC
#error "ROOMEFFECT_GLINT_RIBBON_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_CALLBACK
#error "ROOMEFFECT_GLINT_RIBBON_CALLBACK must name the glint spark particle"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_JOINTS
#error "ROOMEFFECT_GLINT_RIBBON_JOINTS must name the three joint indices"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_OFFSET0
#error "ROOMEFFECT_GLINT_RIBBON_OFFSET0 must name the first joint offset"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_OFFSET1
#error "ROOMEFFECT_GLINT_RIBBON_OFFSET1 must name the last joint offset"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_COLOR0
#error "ROOMEFFECT_GLINT_RIBBON_COLOR0 must name the first ribbon colour"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_COLOR1
#error "ROOMEFFECT_GLINT_RIBBON_COLOR1 must name the middle ribbon colour"
#endif
#ifndef ROOMEFFECT_GLINT_RIBBON_COLOR2
#error "ROOMEFFECT_GLINT_RIBBON_COLOR2 must name the last ribbon colour"
#endif

extern s16 ROOMEFFECT_GLINT_RIBBON_JOINTS[3];
extern GteShortVector ROOMEFFECT_GLINT_RIBBON_OFFSET0;
extern GteShortVector ROOMEFFECT_GLINT_RIBBON_OFFSET1;
extern RenderColor ROOMEFFECT_GLINT_RIBBON_COLOR0;
extern RenderColor ROOMEFFECT_GLINT_RIBBON_COLOR1;
extern RenderColor ROOMEFFECT_GLINT_RIBBON_COLOR2;
extern int ROOMEFFECT_GLINT_RIBBON_CALLBACK(int mode, RoomDampedSpark *spark);

/* Strings two light ribbons across three actor joints.  For twelve frames
 * the last joint and its floor shadow each shed a glint spark per frame,
 * after which the ribbons fade out. */
int ROOMEFFECT_GLINT_RIBBON_FUNC(int mode, RoomGlintRibbon *ribbon) {
    GteShortVector first;
    GteShortVector middle;
    GteShortVector last;
    GteShortVector offset0 = ROOMEFFECT_GLINT_RIBBON_OFFSET0;
    GteShortVector offset1 = ROOMEFFECT_GLINT_RIBBON_OFFSET1;
    RenderColor color0 = ROOMEFFECT_GLINT_RIBBON_COLOR0;
    RenderColor color1 = ROOMEFFECT_GLINT_RIBBON_COLOR1;
    RenderColor color2 = ROOMEFFECT_GLINT_RIBBON_COLOR2;
    RoomDampedSpark *spark;

    switch (mode) {
    case 0:
        ribbon->alpha = 0x80;
        ribbon->reserved00 = 0;
        ribbon->timer = 0;
        func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[0],
                      &offset0, &first);
        func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[1],
                      &offset0, &middle);
        func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[2],
                      &offset1, &last);
        func_800D1384(&first, &middle, 0x3F2, &color0, &color1, 0x80,
                      ribbon->trailA, 1);
        func_800D1384(&middle, &last, 0x3F2, &color1, &color2, 0x80,
                      ribbon->trailB, 1);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x24,
                             ROOMEFFECT_GLINT_RIBBON_CALLBACK);
    case 1:
        ribbon->timer++;
        func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[2],
                      &offset0, &first);
        middle.x = first.x;
        middle.y = first.y;
        middle.z = first.z;
        first.y = D_800942EC.count;
        if (ribbon->timer < 13) {
            spark = func_800CE610(D_800F33E0->pool);
            if (spark != 0) {
                spark->x = first.x;
                spark->y = first.y;
                spark->z = first.z;
                spark->vx = func_80071A54() % 32 - 0x10;
                spark->vy = -(func_80071A54() % 32);
                spark->vz = func_80071A54() % 32 - 0x10;
                spark->state = 0;
                spark->timer = 0;
            }
            spark = func_800CE610(D_800F33E0->pool);
            if (spark != 0) {
                spark->x = middle.x;
                spark->y = middle.y;
                spark->z = middle.z;
                spark->vx = func_80071A54() % 32 - 0x10;
                spark->vy = func_80071A54() % 32 - 0x10;
                spark->vz = func_80071A54() % 32 - 0x10;
                spark->state = 1;
                spark->timer = 0;
            }
        } else if (ribbon->alpha > 0) {
            ribbon->alpha -= 0x10;
        }
        if (ribbon->timer < 12) break;
        return 2;
    case 2:
        if (ribbon->alpha >= 0) {
            func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[0],
                          &offset0, &first);
            func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[1],
                          &offset0, &middle);
            func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_GLINT_RIBBON_JOINTS[2],
                          &offset1, &last);
            func_800D1384(&first, &middle, 10, &color0, &color1, ribbon->alpha,
                          ribbon->trailA, 1);
            func_800D1384(&middle, &last, 10, &color1, &color2, ribbon->alpha,
                          ribbon->trailB, 1);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int tpage = D_800E2850[D_800E11E8];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x10;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
