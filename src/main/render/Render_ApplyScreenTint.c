#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */

typedef struct RgbPrim {
    u8 pad0[4];
    u8 r;
    u8 g;
    u8 b;
    u8 pad7[9];
} RgbPrim;

extern struct { char _[16]; } D_800BCF88_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCFFC_o __asm__("D_800BCFFC");
extern volatile s32 g_ActiveDrawSlot;
extern struct { char _[16]; } D_800B1624_a_o __asm__("D_800B1624");
extern struct { char _[16]; } D_800B1624_b_o __asm__("D_800B1624");

#define D_800BCF88 (*(s32 *)&D_800BCF88_o)
#define D_800BCFFC (*(u8 *)&D_800BCFFC_o)
#define D_800B1624_A (*(u8 **)&D_800B1624_a_o)
#define D_800B1624_B (*(u8 **)&D_800B1624_b_o)
#define READ_S32(base, offset) (*(s32 *)((u8 *)(base) + (offset)))
#define READ_U16(base, offset) (*(u16 *)((u8 *)(base) + (offset)))

int Render_ApplyScreenTint(void) {
    u8 *geom;
    register u8 *entry asm("$4");
    RgbPrim *prim;
    u32 flags;
    int tint;
    int tint_loop;
    int entry_index;
    int prim_index;
    int entry_count;
    int prim_count;
    int active_slot;
    s32 stack_pad[4];

    flags = D_800BCF88;
    if ((flags & 0x1000) == 0) {
        return 0;
    }

    if (flags & 0x2000) {
        tint = D_800BCFFC;
    } else {
        tint = 0x80;
    }

    entry_index = 0;
    geom = D_800B1624_A;
    entry = D_800B1624_B;
    entry = entry + READ_S32(geom, 0x14);
    entry_count = READ_U16(geom, 0x6);
    if (entry_count != 0) {
        tint_loop = tint;
        do {
            prim = (RgbPrim *)READ_S32(entry, 0x30);
            asm("" : : "r"(prim) : "$2");
            active_slot = g_ActiveDrawSlot;
            asm("" : : "r"(active_slot) : "$6");
            prim_count = READ_U16(entry, 0x26);
            asm("" : "=r"(prim_count) : "0"(prim_count));
            if (active_slot != 0) {
                prim = prim + prim_count;
            }
            prim_index = 0;
            if (prim_count != 0) {
                do {
                    prim->b = tint_loop;
                    prim->g = tint_loop;
                    prim->r = tint_loop;
                    asm("" : : : "memory");
                    prim_index++;
                    prim++;
                } while ((u32)prim_index < (u32)prim_count);
            }
            entry_index++;
            entry += 0x38;
        } while (entry_index < entry_count);
    }

    {
        s32 mask;
        s32 *flags_ptr;
        register s32 old_flags asm("$2");
        s32 mode_bits;
        register s32 value asm("$2");

        mask = 0xFFFF3FFF;
        flags_ptr = &D_800BCF88;
        old_flags = *flags_ptr;
        mask = old_flags & mask;
        mode_bits = old_flags & 0xC000;
        value = 0x4000;
        *flags_ptr = mask;
        if (mode_bits == value) {
            goto mode_4000;
        }
        value = 0x8000;
        if (mode_bits == value) {
            goto mode_8000;
        }
        return 0;
mode_4000:
        value = mask | 0x8000;
        goto store_flags;
mode_8000:
        value = -0x1001;
        value = mask & value;
store_flags:
        *flags_ptr = value;
    }

    return 0;
}
