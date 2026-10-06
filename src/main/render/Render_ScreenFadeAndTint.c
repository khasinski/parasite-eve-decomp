/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/render_prim.h"

extern int g_RenderStateFlags;
extern u8 g_RenderFadeFrameCount;
extern u8 g_RenderFadeFrameCounter;
int Render_StartFadeIn(int arg0)
{
  int new_var;
  register int *flags;
  int value;
  flags = &g_RenderStateFlags;
  g_RenderFadeFrameCount = arg0;
  g_RenderFadeFrameCounter = 0;
  new_var = *flags;
  value = new_var;
  value &= ~0xC00;
  value |= 0x400;
  *flags = value;
  return 0;
}

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
    PrimEntry *prim;
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
            prim = (PrimEntry *)READ_S32(entry, 0x30);
            active_slot = D_8009CDDC;
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
                } while ((u32)prim_index < prim_count);
            }
            entry_index++;
            entry += 0x38;
        } while (entry_index < entry_count);
    }

    fade_ptr = D_800BCFFB_PTR;
    frame = *fade_ptr;
    frame++;
    *fade_ptr = frame;
    if (frame < D_800BCFFA) {
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

int Render_BeginSceneLoad(void) {
    u32 *flags;
    u32 temp;
    u32 old;
    u32 value;
    u32 next;
    u32 mask;
    u32 *flags2;
    u32 value2;
    flags = (u32 *)&g_RenderStateFlags;
    old = *flags;
    value = old | 0x1000;
    *flags = value;
    if (value & 0x2000) {
        temp = value & ~0x2000;
        next = temp;
    } else {
        next = old | 0x3000;
    }
    *flags = next;
    asm volatile("" : : : "memory");
    mask = 0xFFFF0000;
    flags2 = (u32 *)&g_RenderStateFlags;
    value2 = *flags2;
    mask |= 0x3FFF;
    value2 &= mask;
    value2 |= 0x4000;
    *flags2 = value2;
    return 0;
}

extern struct { char _[16]; } D_800BCFFC_o __asm__("D_800BCFFC");
extern s32 g_ActiveDrawSlot;

#define D_800BCFFC (*(u8 *)&D_800BCFFC_o)

int Render_ApplyScreenTint(void) {
    u8 *geom;
    register u8 *entry asm("$4");
    PrimEntry *prim;
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
            prim = (PrimEntry *)READ_S32(entry, 0x30);
            active_slot = g_ActiveDrawSlot;
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
                } while ((u32)prim_index < prim_count);
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
