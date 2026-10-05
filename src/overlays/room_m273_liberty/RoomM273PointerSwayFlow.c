/* Pointer sprite pool and boss sway state share D_8019AF74 points. */
#include "room_m273_boss.h"
#include "pe1/gte.h"
typedef GteShortVector Position;
typedef RoomM273PointerPoolEffect Effect;

/* The emitter owns the pointer entries rendered by this callback. */
extern short D_8019AE88;
extern unsigned short D_800F336E, D_800F3370, D_800F3372;

int func_80199950(int mode, Effect *effect) {
    /* Layout only: the original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int frame = (short)(D_800E27EC - 1);
        int kind = D_800F336C;
        unsigned int sizeOffset, shadeOffset;
        int size, shade, palette;
        unsigned short clut;
        sizeOffset = ((unsigned int)frame << 8) & 0x3F00;
        shadeOffset = ((unsigned int)frame << 9) & 0x3E00;
        size = *(int *)((char *)D_800966EC + sizeOffset) + 2048;
        shade = (short)*(int *)((char *)D_800966EC + shadeOffset) >> 6;
        asm("" : "=r"(size), "=r"(shade) : "0"(size), "1"(shade) : "memory");
        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3);
        func_800CEE20(effect->position, 0, (short)size, (short)size,
            64, clut, 1, (short)shade, 0);
    }
    return 0;
}

int func_80199A90(int mode) {
    switch(mode) {
    case 0:
        D_8019AE88=0;
        return func_800CE560(D_800F33E0->pool,4,6,func_80199950);
    case 1: {
        int i;
        Position *position;
        if(D_8019AE88) return 2;
        if(D_8019AF74.animation==15 && D_8019AF74.frame>=25) {
            D_8019AE88=1;
            return 2;
        }
        if(D_800E27EC&7) return 0;
        i=0;
        position=D_8019AF74.points;
        for(;i<2;++i) {
            Effect *output=func_800CE610(D_800F33E0->pool);
            if(!output) break;
            output->position=position;
            ++position;
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11FA;
        unsigned short palette;
        D_800F3368.parameter00=64;
        D_800F336A=4;
        D_800F3376=64;
        D_800F3378=64;
        D_800F3376=64;
        D_800F3378=64;
        palette=D_800E2850[index];
        asm("" : : "r"(palette) : "memory", "$2");
        D_800F336C=3;
        D_800F336E=1;
        D_800F3372=0;
        D_800F3374=32;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}

/* Animation 15 controller: transforms two model points, then sways the
 * boss toward or away from the player for eight frames. */
int func_80199C50(int mode) {
    RoomM273BossInstance *instance = D_800F32D0->instance;
    GteShortVector offset;
    int value; /* loop index, then the player heading */
    int frame;
    int frame_1A;
    GteShortVector *point;

    if (mode == 0) {
        D_8019AF74.done = 0;
        instance->owner->flags |= 0x40000000;
        D_8019AF74.points[0].pad = 0;
        D_8019AF74.points[1].pad = 1;
        D_8019AF74.hits[0].pad = 0;
        D_8019AF74.hits[1].pad = 0;
        D_8019AF74.sway_timer = 0;
        D_8019AF74.cooldown = 0;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        frame = instance->time.parts.frame;
        frame_1A = instance->previous.parts.frame;
        D_8019AF74.animation = instance->animation;
        D_8019AF74.frame = frame;
        D_8019AF74.frame_1A = frame_1A;
        offset.x = 0;
        offset.y = 0;
        offset.z = -0x130;
        point = D_8019AF74.points;
        for (value = 0; value < 2; value++) {
            GteMatrix *m = instance->transforms;
            if (value) m += 36;
            else m += 25;
            gte_ldrotmatrix(m);
            gte_ldtransmatrix(m);
            gte_ldv0(&offset);
            gte_rtv0tr_mac();
            gte_stsv(&point[value]);
        }
        if (instance->animation == 15) {
            if ((s16)frame >= instance->frame_count - 1) {
                D_8019AF74.done = 1;
                if (instance->owner) *instance->owner->status = 4;
                return 1;
            }
            if ((s16)frame >= 0x16) {
                if (D_8019AF74.repeat-- > 0) instance->time.fixed = 0;
            }
        }
        if (D_8019AF74.repeat && (s16)frame >= 0xF && (s16)frame_1A < 0xF) {
            int step;
            D_8019AF74.sway_timer = 8;
            value = (s16)FieldEng_VecToAngle(g_PlayerEntity->position, instance->position);
            step = 0x20;
            if ((value - instance->yaw) & 0x800) step = -0x20;
            D_8019AF74.sway_step = step;
        }
        if (D_8019AF74.sway_timer) {
            instance->yaw += D_8019AF74.sway_step *
                             D_800966EC[(D_8019AF74.sway_timer << 7) & 0xF80].sine / 4096;
            D_8019AF74.sway_timer--;
        }
        if (D_8019AF74.cooldown) D_8019AF74.cooldown--;
    }
    return 0;
}
