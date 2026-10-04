/* Motion particle setup (rooms m075, m080, m082). FUNC names the entry. */
#include "pe1/room_motion_init.h"

void FUNC(RoomMotionInitOwner *owner, void *unused, RoomMotionInitState *state) {
    RoomMotionInitLink *link;
    RoomMotionParticle *particle;
    unsigned int i;

    func_800C2B40(state);
    state->allocation = func_8006DC18(12);
    link = owner->link;
    state->link = link;
    state->base = link->model->base;
    state->primary = state->link->model->primary;
    state->secondary = state->link->model->secondary;
    state->timer = 30;
    state->field66 = 0;
    state->field68 = 0;

    D_80194618[1].size = 44;
    D_80194618[1].visible = 1;
    D_80194618[1].x = -30;
    D_80194618[1].y = 0;
    D_80194618[1].red = 128;
    D_80194618[1].green = 128;
    D_80194618[1].blue = 128;
    D_80194618[1].flags = 0;

    D_80194618[2].size = 44;
    D_80194618[2].visible = 1;
    D_80194618[2].x = 0;
    D_80194618[2].y = 32;
    D_80194618[2].red = 128;
    D_80194618[2].green = 128;
    D_80194618[2].blue = 128;
    D_80194618[2].flags = 0;

    D_80194618[0].size = 44;
    D_80194618[0].visible = 1;
    D_80194618[0].x = -30;
    D_80194618[0].y = 128;
    D_801940B8.x = -31;
    D_80194618[3].size = 32;
    D_80194618[0].red = 128;
    D_80194618[0].green = 128;
    D_80194618[0].blue = 128;
    D_80194618[0].flags = 0;

    D_801940B8.size = 0;
    D_801940B8.visible = 1;
    D_801940B8.y = 128;
    D_801940B8.red = 128;
    D_801940B8.green = 128;
    D_801940B8.blue = 128;
    D_801940B8.flags = 0;

    D_80194618[3].visible = 1;
    D_80194618[3].x = 100;
    D_80194618[3].y = 128;
    D_80194618[3].red = 128;
    D_80194618[3].green = 128;
    D_80194618[3].blue = 128;
    D_80194618[3].flags = 0;

    for (i = 0; i < 10; i++) {
        particle = &D_801940C8[i];
        particle->mode = 4;
        particle->alpha = 128;
        particle->kind = 4;
        particle->life = 30;
        particle->speed = 0;
        if (i == 0) particle->drift = -64;
        else particle->drift = 0;
        particle->red = 255;
        particle->green = 255;
        particle->blue = 255;
        particle = &D_80194370[i];
        particle->mode = 4;
        particle->alpha = 128;
        particle->kind = 4;
        particle->life = 30;
        particle->speed = 0;
        if (i == 0) particle->drift = 64;
        else particle->drift = 0;
        particle->red = 255;
        particle->green = 255;
        particle->blue = 255;
    }
}

#undef FUNC
