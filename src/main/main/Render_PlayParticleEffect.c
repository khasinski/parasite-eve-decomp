typedef unsigned char u8;
typedef int s32;

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

void Render_PlayParticleEffect(int type, void *payload) {
    int kind;
    ParticleEffectSlot *slot;

    if ((type & 0xFF) == 5) {
        if ((*(u8 *)payload & 0x10) != 0) {
            Render_DrawParticle(5, payload);
        }
    }

    kind = type & 0xFF;
    slot = 0;
    if (kind == 4) {
        slot = &D_800A3530;
    } else if ((kind == 1) || (kind == 5)) {
        slot = &D_800A3520;
    }

    if (slot != 0) {
        slot->state = 1;
        slot->type = type;
        Util_Copy8(slot->payload, payload);
    }

    if (D_800B8AB4 != 0) {
        D_800B8AB4(type & 0xFF, payload);
    }
}
