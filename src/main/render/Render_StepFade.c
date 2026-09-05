#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: --expand-div */

typedef struct RgbPrim {
    u8 pad0[4];
    u8 r;
    u8 g;
    u8 b;
    u8 pad7[9];
} RgbPrim;

extern struct { char _[16]; } D_800BCF88_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCF88_store_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCFFA_o __asm__("D_800BCFFA");
extern struct { char _[16]; } D_800BCFFB_o __asm__("D_800BCFFB");
extern s32 D_8009CDDC;
extern struct { char _[16]; } D_800B1624_a_o __asm__("D_800B1624");
extern struct { char _[16]; } D_800B1624_b_o __asm__("D_800B1624");
extern struct { char _[16]; } D_800B1624_c_o __asm__("D_800B1624");

#define D_800BCF88 (*(s32 *)&D_800BCF88_o)
#define D_800BCFFA (*(u8 *)&D_800BCFFA_o)
#define D_800BCFFB (*(u8 *)&D_800BCFFB_o)
#define D_800BCFFB_PTR ((u8 *)&D_800BCFFB_o)
#define D_800B1624_A (*(u8 **)&D_800B1624_a_o)
#define D_800B1624_B (*(u8 **)&D_800B1624_b_o)
#define D_800B1624_C (*(u8 **)&D_800B1624_c_o)
#define READ_S32(base, offset) (*(s32 *)((u8 *)(base) + (offset)))
#define READ_U16(base, offset) (*(u16 *)((u8 *)(base) + (offset)))
#define WRITE_U16(base, offset, value) (*(u16 *)((u8 *)(base) + (offset)) = (value))

int Render_StepFade(void) {
    u8 *geom;
    register u8 *entry asm("$4");
    RgbPrim *prim;
    s32 fade_step;
    int fade_value;
    int tint_loop;
    int entry_index;
    int prim_index;
    int entry_count;
    int prim_count;
    int active_slot;
    register s32 flags asm("$2");
    s32 mask;
    s32 divisor;
    u8 *final_geom;
    u8 *fade_ptr;
    u8 frame;
    s32 stack_pad[4];

    if ((D_800BCF88 & 0x400) == 0) {
        return 0;
    }

    fade_step = D_800BCFFB;
    divisor = D_800BCFFA;
    fade_step <<= 7;
    divisor--;
    fade_step /= divisor;

    entry_index = 0;
    geom = D_800B1624_A;
    entry = D_800B1624_B;
    fade_value = READ_S32(geom, 0x14);
    entry_count = READ_U16(geom, 0x6);
    entry = entry + fade_value;
    fade_value = 0x80 - fade_step;
    if (entry_count != 0) {
        tint_loop = fade_value;
        do {
            prim = (RgbPrim *)READ_S32(entry, 0x30);
            asm("" : : "r"(prim) : "$2");
            active_slot = D_8009CDDC;
            asm("" : : "r"(active_slot) : "$6");
            prim_count = READ_U16(entry, 0x26);
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

    fade_ptr = D_800BCFFB_PTR;
    frame = *fade_ptr;
    frame++;
    *fade_ptr = frame;
    if ((u32)frame < (u32)D_800BCFFA) {
        return 0;
    }

    flags = D_800BCF88;
    mask = -0xC01;
    flags &= mask;
    final_geom = D_800B1624_C;
    flags |= 0x800;
    *(s32 *)&D_800BCF88_store_o = flags;
    flags = 0x1FF0;
    WRITE_U16(final_geom, 0x26, flags);

    return 0;
}
