#include "common.h"

typedef struct ParticleEffectSlot {
    s32 state;
    u8 type;
    u8 payload[8];
} ParticleEffectSlot;

extern ParticleEffectSlot D_800A3520;
extern ParticleEffectSlot D_800A3530;
extern void (*D_800B8AB4)(int type, void *payload);

void Render_DrawParticle(int type, void *payload);
void Util_Copy8(void *dst, void *src);

void Render_PlayParticleEffect(char type, void *payload) {
    ParticleEffectSlot *slot;
    int kind;

    if (((type & 0xFF) == 5) && ((*(u8 *)payload & 0x10) != 0)) {
        Render_DrawParticle(5, payload);
    }

    kind = type & 0xFF;
    switch (kind) {
    case 1:
    case 5:
        slot = &D_800A3520;
        break;
    case 4:
        slot = &D_800A3530;
        break;
    default:
        goto call_handler;
    }

    slot->state = 1;
    slot->type = type;
    Util_Copy8(slot->payload, payload);

call_handler:
    if (D_800B8AB4 != 0) {
        D_800B8AB4(type & 0xFF, payload);
    }
}
