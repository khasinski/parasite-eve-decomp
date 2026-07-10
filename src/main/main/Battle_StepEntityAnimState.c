typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } g_FieldActorListHead_o __asm__("g_FieldActorListHead");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } g_ActiveActor_o __asm__("g_ActiveActor");
extern struct { char _[16]; } D_8009D2A0_o __asm__("D_8009D2A0");
extern struct { char _[16]; } D_8009D21C_o __asm__("D_8009D21C");
extern struct { char _[16]; } D_8009D304_o __asm__("D_8009D304");
extern struct { char _[16]; } D_800A7FF0_o __asm__("D_800A7FF0");

#define g_FieldActorListHead (*(u8 **)&g_FieldActorListHead_o)
#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define g_ActiveActor (*(u8 **)&g_ActiveActor_o)
#define D_8009D2A0 (*(u8 *)&D_8009D2A0_o)
#define D_8009D21C (*(u16 *)&D_8009D21C_o)
#define D_8009D304 (*(u32 *)&D_8009D304_o)

#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S8_AT(ptr, offset) (*(s8 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define PTR_AT(ptr, offset) (*(u8 **)((u8 *)(ptr) + (offset)))

#define ENTITY_CORE(entity) PTR_AT(entity, 0)
#define ENTITY_NEXT(entity) PTR_AT(entity, 4)
#define ENTITY_FLAGS(entity) U32_AT(entity, 0x98)
#define ENTITY_OWNER(entity) PTR_AT(entity, 0x18C)
#define ENTITY_ANIM(entity) ((u8 *)(entity) + 0x1B4)
#define ENTITY_HIDE_FLAGS(entity) U16_AT(entity, 0x250)
#define REWARD_SLOT(index) (*(s16 *)((u8 *)&D_800A7FF0_o + ((u8)(index) << 2)))
#define REWARD_SLOT_EXTRA(index) (*(s16 *)((u8 *)&D_800A7FF0_o + ((u8)(index) << 2) + 2))

void Anim_SetInterpRate(void *anim, int rate);
void Akao_Cmd_21(int arg0, int arg1);
void Render_FadeEntityColor(void *anim, int r, int g, int b);
void Asset_Find08w(int id, int arg1, int x, int y, int z);
void Battle_SlotFree(void *entity);
void Battle_StartDeathAnim(void);

void Battle_StepEntityAnimState(void *entity) {
    u8 *state;
    u8 *iter;
    int color;
    int product;
    int diff;
    int i;
    int all_done;

    state = ENTITY_CORE(entity);

    switch (U8_AT(state, 0xAC)) {
    case 0:
        D_8009D2A0--;
        U8_AT(state, 0xAC)++;
        /* fallthrough */
    case 1:
        if (U8_AT(state, 0xAF) != 0) {
            if (U16_AT(entity, 0x16) < U8_AT(state, 0x91) &&
                U8_AT(entity, 0xE) == 0 &&
                S8_AT(state, 5) < 2) {
                return;
            }

            ENTITY_FLAGS(entity) |= 0x100;
            Anim_SetInterpRate(ENTITY_ANIM(entity), 0x16);
            U8_AT(state, 0xAD) = 0;
            U8_AT(state, 0xAC)++;
            U32_AT(entity, 0x68) = 0;
            U32_AT(entity, 0x6C) = 0;
            U32_AT(entity, 0x70) = 0;
            ENTITY_FLAGS(entity) |= 0x1000;
            Akao_Cmd_21(0, U32_AT(state, 8));

            iter = g_FieldActorListHead;
            while (iter != 0) {
                if (ENTITY_CORE(iter) == 0 && ENTITY_OWNER(iter) == entity) {
                    Anim_SetInterpRate(ENTITY_ANIM(iter), 0x16);
                }
                iter = ENTITY_NEXT(iter);
            }
        } else {
            Akao_Cmd_21(0, U32_AT(state, 8));
            U8_AT(state, 0xAC) = 3;
        }
        break;

    case 2:
        color = (-0x80 - (U8_AT(state, 0xAD) << 4)) & 0xFF;
        Render_FadeEntityColor(ENTITY_ANIM(entity), color, 0x80, color);
        if (color == 0) {
            ENTITY_HIDE_FLAGS(entity) |= 2;
            U8_AT(state, 0xAD) = 0;
            U8_AT(state, 0xAC)++;
        } else if (U8_AT(state, 0xAD) == 0 && U8_AT(state, 0xAE) != 0) {
            Asset_Find08w(
                U16_AT(state, 0xB4),
                0,
                S16_AT(entity, 0x268),
                S16_AT(entity, 0x26A),
                S16_AT(entity, 0x26C));
        }
        U8_AT(state, 0xAD)++;

        iter = g_FieldActorListHead;
        while (iter != 0) {
            if (ENTITY_CORE(iter) == 0 && ENTITY_OWNER(iter) == entity) {
                Render_FadeEntityColor(ENTITY_ANIM(iter), color, 0x80, color);
                if (color == 0) {
                    ENTITY_HIDE_FLAGS(iter) |= 2;
                }
            }
            iter = ENTITY_NEXT(iter);
        }
        break;

    case 3:
        if (U8_AT(entity, 0x252) != 0 && U8_AT(state, 0xAF) != 0) {
            return;
        }

        ENTITY_FLAGS(entity) |= 0x410;
        Battle_SlotFree(entity);

        iter = g_FieldActorListHead;
        while (iter != 0) {
            if (ENTITY_CORE(iter) == 0 && ENTITY_OWNER(iter) == entity) {
                ENTITY_FLAGS(iter) |= 0x10;
            }
            iter = ENTITY_NEXT(iter);
        }

        product = U16_AT(state, 0x9A) * U16_AT(state, 0x9C);
        D_8009D304 += U16_AT(state, 0xE);
        diff = U16_AT(state, 0x98) - product;
        if (diff > 0) {
            D_8009D21C += diff;
        }

        for (i = 0; (u8)i < 10; i++) {
            if (REWARD_SLOT(i) == 0) {
                REWARD_SLOT(i) = U8_AT(state, 0x9E);
                break;
            }
        }

        if (S16_AT(state, 0xA0) != 0) {
            for (i = 0; (u8)i < 10; i++) {
                if (REWARD_SLOT(i) == 0) {
                    REWARD_SLOT(i) = U16_AT(state, 0xA0);
                    if (S16_AT(state, 0xA2) >= 0) {
                        REWARD_SLOT_EXTRA(i) = S16_AT(state, 0xA2);
                    }
                    break;
                }
            }
        }

        if ((s8)D_8009D2A0 != 0) {
            return;
        }

        iter = g_FieldActorListHead;
        all_done = 1;
        while (iter != 0) {
            if (iter != D_8009D254 && ENTITY_CORE(iter) != 0) {
                all_done = 0;
            }
            iter = ENTITY_NEXT(iter);
        }

        if (all_done && S16_AT(g_ActiveActor, 0xC) > 0) {
            Battle_StartDeathAnim();
        }
        break;
    }
}
