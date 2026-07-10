/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct RgbPrim {
    u8 pad0[4];
    u8 r;
    u8 g;
    u8 b;
    u8 pad7[9];
} RgbPrim;

extern struct { char _[16]; } D_800BCF88_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCFFC_o __asm__("D_800BCFFC");
extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_800B1624_a_o __asm__("D_800B1624");
extern struct { char _[16]; } D_800B1624_b_o __asm__("D_800B1624");

#define D_800BCF88 (*(s32 *)&D_800BCF88_o)
#define D_800BCFFC (*(u8 *)&D_800BCFFC_o)
#define g_ActiveDrawSlot (*(volatile s32 *)&D_8009CDDC_o)
#define D_800B1624_A (*(u8 **)&D_800B1624_a_o)
#define D_800B1624_B (*(u8 **)&D_800B1624_b_o)
#define READ_S32(base, offset) (*(s32 *)((u8 *)(base) + (offset)))
#define READ_U16(base, offset) (*(u16 *)((u8 *)(base) + (offset)))

int Render_ApplyScreenTint(void) {
    register u8 *geom asm("$2");
    register u8 *entry asm("$4");
    register RgbPrim *prim asm("$3");
    u32 flags;
    register int tint asm("$5");
    register int tint_loop asm("$7");
    register int entry_index asm("$8");
    register int prim_index asm("$5");
    register int entry_count asm("$9");
    register int prim_count asm("$6");
    register int active_slot asm("$2");
    s32 stack_pad[4];

    flags = D_800BCF88;
    if ((flags & 0x1000) == 0) {
        return 0;
    }
    asm volatile("" : : "r"(stack_pad));

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
            asm volatile(
                "lui\t%0,%%hi(g_ActiveDrawSlot)\n\t"
                "lw\t%0,%%lo(g_ActiveDrawSlot)(%0)\n\t"
                "lhu\t%1,0x26(%2)"
                : "=r"(active_slot), "=r"(prim_count)
                : "r"(entry));
            if (active_slot != 0) {
                prim = prim + prim_count;
            }
            prim_index = 0;
            if (prim_count != 0) {
                do {
                    asm volatile(
                        "sb\t%1,0x6(%0)\n\t"
                        "sb\t%1,0x5(%0)\n\t"
                        "sb\t%1,0x4(%0)"
                        :
                        : "r"(prim), "r"(tint_loop)
                        : "memory");
                    prim_index++;
                    prim++;
                } while ((u32)prim_index < (u32)prim_count);
            }
            entry_index++;
            entry += 0x38;
        } while (entry_index < entry_count);
    }

    {
        register s32 mask asm("$3");
        register s32 *flags_ptr asm("$5");
        register s32 old_flags asm("$2");
        register s32 mode_bits asm("$4");
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
