#include "pe1/field_anim.h"
#include "pe1/render_object.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"

#include "pe1/field_actor.h"
int func_800DA5D4(int mode, FieldAnimEmitter *state) {
    FieldAnimEmittedPoint *point;
    int angle;

    switch (mode) {
    case 0:
        state->angle = rand();
        func_800C6D5C(D_800F32D8, 0, 0);
        return func_800CE560(D_800F33E0->end, 16, 24, func_800DA1FC);
    case 1:
        if (D_800E27EC < 40 && D_800E27EC % 6 == 0) {
            point = func_800CE610(D_800F33E0->end);
            if (point) {
                angle = state->angle;
                point->state = 0;
                point->phase = 0;
                point->angle = angle;
                state->angle -= 1365 + (rand() & 255);
            }
        }
        if (D_800E27EC >= 70)
            return 1;
        break;
    case 2:
        D_800E2214[0] = D_800F32D0->actor->render_object.target_x;
        D_800E2214[1] = D_800F32D0->actor->render_object.target_y;
        D_800E2214[2] = D_800F32D0->actor->render_object.target_z;
        D_800E2214[1] = D_800942EC;
        break;
    }
    return 0;
}
