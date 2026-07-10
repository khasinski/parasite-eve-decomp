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
extern struct { char _[16]; } D_800BCFFA_o __asm__("D_800BCFFA");
extern struct { char _[16]; } D_800BCFFB_o __asm__("D_800BCFFB");
extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
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
    register u8 *geom asm("$5");
    register u8 *entry asm("$4");
    register RgbPrim *prim asm("$3");
    register s32 fade_step asm("$3");
    register int fade_value asm("$2");
    register int tint_loop asm("$7");
    register int entry_index asm("$8");
    register int prim_index asm("$5");
    register int entry_count asm("$9");
    register int prim_count asm("$6");
    register int active_slot asm("$2");
    s32 flags;
    u8 *fade_ptr;
    u8 frame;
    s32 stack_pad[4];

    if ((D_800BCF88 & 0x400) == 0) {
        return 0;
    }
    asm volatile("" : : "r"(stack_pad));

    asm volatile(
        ".set\tnoreorder\n\t"
        ".set\tnoat\n\t"
        "lui\t$3,%%hi(D_800BCFFB)\n\t"
        "lbu\t$3,%%lo(D_800BCFFB)($3)\n\t"
        "lui\t$2,%%hi(D_800BCFFA)\n\t"
        "lbu\t$2,%%lo(D_800BCFFA)($2)\n\t"
        "sll\t$3,$3,7\n\t"
        "addiu\t$2,$2,-1\n\t"
        "div\t$zero,$3,$2\n\t"
        "bnez\t$2,1f\n\t"
        "nop\n\t"
        "break\t7168\n"
        "1:\n\t"
        "addiu\t$at,$zero,-1\n\t"
        "bne\t$2,$at,2f\n\t"
        "lui\t$at,0x8000\n\t"
        "bne\t$3,$at,2f\n\t"
        "nop\n\t"
        "break\t6144\n"
        "2:\n\t"
        "mflo\t$3\n\t"
        ".set\tat\n\t"
        ".set\treorder"
        : "=r"(fade_step)
        :
        : "$1", "$2", "hi", "lo", "memory");

    entry_index = 0;
    geom = D_800B1624_A;
    entry = D_800B1624_B;
    fade_value = READ_S32(geom, 0x14);
    entry_count = READ_U16(geom, 0x6);
    entry = entry + fade_value;
    fade_value = 0x80;
    if (entry_count != 0) {
        fade_value = fade_value - fade_step;
        tint_loop = fade_value;
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
    } else {
        fade_value = fade_value - fade_step;
    }

    fade_ptr = D_800BCFFB_PTR;
    frame = *fade_ptr;
    frame++;
    *fade_ptr = frame;
    if ((u32)frame < (u32)D_800BCFFA) {
        return 0;
    }

    asm volatile(
        ".set\tnoat\n\t"
        "lui\t$2,%%hi(D_800BCF88)\n\t"
        "lw\t$2,%%lo(D_800BCF88)($2)\n\t"
        "addiu\t$3,$zero,-0xC01\n\t"
        "and\t$2,$2,$3\n\t"
        "lui\t$3,%%hi(D_800B1624)\n\t"
        "lw\t$3,%%lo(D_800B1624)($3)\n\t"
        "ori\t$2,$2,0x800\n\t"
        "lui\t$at,%%hi(D_800BCF88)\n\t"
        "sw\t$2,%%lo(D_800BCF88)($at)\n\t"
        "addiu\t$2,$zero,0x1FF0\n\t"
        "sh\t$2,0x26($3)\n\t"
        ".set\tat"
        :
        :
        : "$1", "$2", "$3", "memory");

    return 0;
}
