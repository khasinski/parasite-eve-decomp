/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D254_a __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_b __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_c __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_d __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_e __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_a __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_b __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_c __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_d __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D1A0_r0 __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_w0 __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_r1 __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_w1 __asm__("D_8009D1A0");
extern u8 D_8009D1D4;
extern u16 D_800BE834[];

#define D254_A (*(u8 **)&D_8009D254_a)
#define D254_B (*(u8 **)&D_8009D254_b)
#define D254_C (*(u8 **)&D_8009D254_c)
#define D254_D (*(u8 **)&D_8009D254_d)
#define D254_E (*(u8 **)&D_8009D254_e)
#define D278_A (*(u8 **)&D_8009D278_a)
#define D278_B (*(u8 **)&D_8009D278_b)
#define D278_C (*(u8 **)&D_8009D278_c)
#define D278_D (*(u8 **)&D_8009D278_d)
#define D1A0_R0 (*(int *)&D_8009D1A0_r0)
#define D1A0_W0 (*(int *)&D_8009D1A0_w0)
#define D1A0_R1 (*(int *)&D_8009D1A0_r1)
#define D1A0_W1 (*(int *)&D_8009D1A0_w1)

#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define ACTION_AT(index) (*(u16 *)((u8 *)D_800BE834 + ((u8)(index) << 3)))

void Entity_SetActionMode(void *entity, int mode);
void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Battle_ApplyDamage(int action);
int Scene_LoadRoomAssets(int room, void *entity);

void Battle_ApplyPlayerHit(void) {
    register u32 committed asm("$16");
    register int mask asm("$5");
    register unsigned int index asm("$3");
    u8 *actor;
    int flags;
    u16 action;

    committed = 0x200000;
    if (U32_AT(D278_A, 0x4C) & committed) {
        Entity_SetActionMode(D254_A, 0xE);
        Asset_Find08Alt(0x4B3, 0, S16_AT(D254_B, 0x2A), S16_AT(D254_B, 0x2E), S16_AT(D254_B, 0x32));

        mask = ~0x200000;
        actor = D278_B;
        index = U32_AT(actor, 0x4C);
        flags = D1A0_R0;
        D1A0_W0 = flags | 0x100;
        U32_AT(actor, 0x4C) = index & mask;
    }

    if (U8_AT(D254_C, 0xF) == U16_AT(D254_C, 0x1A)) {
        Entity_SetActionMode(D254_C, U8_AT(D278_C, 0x12));
        actor = D278_D;
        index = D_8009D1D4;
        U32_AT(actor, 0x4C) |= committed;

        Battle_ApplyDamage((s16)ACTION_AT(index) - 3);
        Scene_LoadRoomAssets(0x55, D254_D);

        index = D_8009D1D4;
        index++;
        action = ACTION_AT(index);

        D_8009D1D4 = index;

        if ((unsigned int)(action - 3) >= 0x194 || U8_AT(D254_E, 0xE) < 4) {
            D1A0_W1 = D1A0_R1 & ~0x100;
        }
    }
}
