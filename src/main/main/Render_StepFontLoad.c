typedef unsigned char u8;
typedef int s32;

void Render_LoadFontGlyph(u8 code);

extern u8 D_80091A1D;
extern u8 D_80091A1E;
extern u8 D_80091A1F;
extern u8 D_80091A1F_rd[] __asm__("D_80091A1F");
extern u8 *D_80091A28;
extern u8 D_8009EE22[];

u8 Render_StepFontLoad(void) {
    u8 state;
    u8 next;
    register u8 code asm("$4");
    u8 *hdr;
    u8 *p;
    u8 *q;
    u8 *t;
    register s32 slot asm("$2");
    register s32 digit asm("$3");
    register s32 i asm("$5");
    register s32 count asm("$6");
    register s32 found asm("$7");
    u8 *statep;
    int stack_pad[3];

    statep = &D_80091A1D;
    state = *statep;
    if (state < 3U) {
        *statep = 1;
        return 0xFFU;
    }

    next = state - 1;
    *statep = next;
    code = D_8009EE22[next];
    D_80091A1E = code;
    Render_LoadFontGlyph(code);

    /* GCC otherwise shifts the modulo-by-10 expansion into different registers. */
    __asm__ volatile(
        "lui\t%0,%%hi(D_80091A1D)\n\t"
        "lbu\t%0,%%lo(D_80091A1D)(%0)\n\t"
        "lui\t%4,0xCCCC\n\t"
        "ori\t%4,%4,0xCCCD\n\t"
        "multu\t%0,%4\n\t"
        "addu\t%3,$zero,$zero\n\t"
        "addu\t%1,$zero,$zero\n\t"
        "mfhi\t$8\n\t"
        "srl\t$4,$8,3\n\t"
        "sll\t%4,$4,2\n\t"
        "addu\t%4,%4,$4\n\t"
        "sll\t%4,%4,1\n\t"
        "subu\t%0,%0,%4\n\t"
        "andi\t%0,%0,0xFF\n\t"
        "sltu\t%0,$zero,%0"
        : "=&r"(digit), "=r"(i), "=r"(count), "=r"(found), "=&r"(slot)
        :);

    hdr = D_80091A28;
    count = hdr[3];
    p = hdr + 1;
    if (count > 0) {
        digit &= 0xFF;
        do {
            q = p + i;
            if (q[3] == digit) {
                found = i;
                i = p[2];
            }
            i++;
        } while (i < count);
        i = 0;
    }

    {
        register s32 count2 asm("$3");
        register s32 wanted asm("$4");
        register u8 *p2 asm("$6");

        slot = p[0x1B];
        p2 = p + 0x1B;
        if (slot > 0) {
            wanted = found & 0xFF;
            count2 = slot;
            do {
                q = p2 + i;
                if (q[1] == wanted) {
                    slot = i;
                    goto store;
                }
                i++;
            } while (i < count2);
        }
    }
    slot = 0xFF;

store:
    __asm__ volatile("" : "=m"(stack_pad));
    D_80091A1F = slot;
    __asm__ volatile("" : : : "memory");
    t = D_80091A28;
    return *(t + *(t + D_80091A1F_rd[0] + 0x1D) + 4);
}
