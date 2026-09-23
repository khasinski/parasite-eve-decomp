#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/geom_state.h"
#include "room_m273.h"

extern FieldActor *g_PlayerEntity;
extern int *D_8009D248;
extern u16 D_8009D1CC;

int func_8001CAB0(int x, int z, int *script, u16 mode);
void func_8001AA78(FieldActor *actor);
s32 func_80192D8C(char *obj);

void func_80192C00(RoomPlacementOwner *owner, RoomPlacementState *state) {
    register FieldActor *player asm("$4") = g_PlayerEntity;
    RoomPlacementMap *placement;
    int *script = D_8009D248;

    player->render_object.animation_source = 0;
    player->render_object.animation_state = 0;
    player->flags &= ~0x10000;
    player->render_object.flags_9C &= ~0x400;

    placement = owner->map;
    if (func_8001CAB0(placement->x << 16, placement->z << 16,
                      script, D_8009D1CC)) {
        g_PlayerEntity->pos_x = owner->map->x << 16;
        g_PlayerEntity->pos_z = owner->map->z << 16;
    } else {
        g_PlayerEntity->pos_x = state->fallback_x;
        g_PlayerEntity->pos_z = state->fallback_z;
    }

    player = g_PlayerEntity;
    func_8001AA78(player);

    {
        FieldActor *updatedPlayer = g_PlayerEntity;
        GeomScrollState *scrollState;

        updatedPlayer->base_x = updatedPlayer->pos_x;
        scrollState = &D_800BCF88;
        updatedPlayer->base_y = updatedPlayer->pos_y;
        updatedPlayer->base_z = updatedPlayer->pos_z;
        updatedPlayer->motion_x = 0;
        updatedPlayer->motion_y = 0;
        updatedPlayer->motion_z = 0;
        updatedPlayer->accel_x = 0;
        updatedPlayer->accel_y = 0;
        updatedPlayer->accel_z = 0;
        scrollState->flags |= 0x80;
        state->active = 0;
    }
}

void func_80192D50(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (*(unsigned short *)(slot + 0x16) >= 0x3B) {
        func_80192D8C(obj);
    }
}
