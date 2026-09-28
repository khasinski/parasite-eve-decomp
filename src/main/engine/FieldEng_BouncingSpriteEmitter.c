#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"
#include "pe1/random.h"
#include "pe1/gte.h"

extern u16 D_800E11E4;

int func_800D6E3C(int mode, s32 *angle) {
    RenderBouncingSprite *particle;
    volatile FieldAnimObjectPrefix *owner;
    volatile GteShortVector position;
    int count;
    int amplitude;
    int product;
    u32 seed;
    int random_step;
    u16 x;
    u16 page;
    FieldAnimTaskSlot *slot;

    switch (mode) {
    case 0:
        seed = rand();
        slot = D_800F33E0;
        *angle = seed;
        return func_800CE560(slot->end, 16, 24,
                              (FieldAnimTaskCallback)func_800D6C58);
    case 1:
        if (D_800E27EC < 8) {
            asm volatile("" ::: "memory");
            for (count = 0; count < 3; count++) {
                particle = func_800CE610(D_800F33E0->end);
                if (particle != 0) {
                    owner = D_800F32D0;
                    x = owner->actor->render_object.target_x;
                    position.x = x;
                    position.y = owner->actor->render_object.target_y;
                    position.z = owner->actor->render_object.target_z;
                    particle->x = x;
                    particle->y = (u16)position.y;
                    particle->z = (u16)position.z;
                    random_step = rand();
                    seed = *angle;
                    seed += 0x955;
                    seed += random_step & 31;
                    *angle = seed;
                    amplitude = (rand() & 127) + 70;
                    product = rsin(*angle) * amplitude;
                    if (product < 0)
                        product += 4095;
                    particle->velocity_x = product >> 12;
                    product = rcos(*angle) * amplitude;
                    if (product < 0)
                        product += 4095;
                    particle->velocity_z = product >> 12;
                    particle->velocity_y = -(rand() & 31) - 36;
                    particle->duration = (rand() & 7) + 26;
                    particle->angle = rand();
                }
            }
        }
        if (D_800E27EC >= 40) {
            asm volatile("" ::: "memory");
            return 1;
        }
        return 0;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        page = D_800E2850[D_800E11E4];
        D_800F3368.palette = 0;
        D_800F3368.tpage = page;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}
