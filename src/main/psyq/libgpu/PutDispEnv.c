/* ASSEMBLER: GNU */
#include "pe1/gpu_state.h"
#include "pe1/psyq_gpu.h"
extern unsigned char D_8009574E, D_8009574F;
extern DISPENV D_800957B8;
extern unsigned short D_800957C0[4];
extern unsigned char D_80095848[];
extern unsigned short D_80095820[], D_80095822[];
extern char D_80011970[];
int GetVideoMode(void);
void *memcpy(void *, const void *, unsigned);
#define SIGNED_HALF(p) ((short)*(volatile unsigned short *)(p))
DISPENV *PutDispEnv(DISPENV *env) {
    register DISPENV *e = env;
    register unsigned char *state;
    register unsigned mode;
    register int vstart, vend;
    register unsigned short *rect;
    int width;
    register int span;
    register int index, hstart, hend, clamp;
    asm("" : "=r"(e) : "0"(e));
    state = &D_8009574E;
    asm("" : "=r"(state) : "0"(state));
    mode = 0x08000000;
    if (*state >= 2)
        D_80095748(D_80011970, e);
    {
        register unsigned opcode = 0x05000000;
        register unsigned y asm("$3") = (unsigned short)e->disp.y;
        register unsigned x asm("$4") = (unsigned short)e->disp.x;
        register GpuCallbacks *ops asm("$5") = D_80095744;
        y = (y & 0x3ff) << 10;
        x = (x & 0x3ff) | opcode;
        ops->callback10(y | x);
    }
    rect = (unsigned short *)(state + 0x6a);
    if (*(unsigned *)(state + 0x7a) != *(unsigned *)&e->isinter ||
        SIGNED_HALF(rect) != e->disp.x || ({
                                              asm("" : "=r"(rect) : "0"(rect));
                                              SIGNED_HALF(rect + 1);
                                          }) != e->disp.y ||
        SIGNED_HALF(rect + 2) != e->disp.w || SIGNED_HALF(rect + 3) != e->disp.h) {
        e->pad0 = GetVideoMode();
        if (e->pad0 == 1)
            mode |= 8;
        if (e->isrgb24)
            mode |= 0x10;
        if (e->isinter)
            mode |= 0x20;
        if (D_8009574F)
            mode |= 0x80;
        width = e->disp.w;
        if (width <= 280) {
        } else if (width <= 352)
            mode |= 1;
        else if (width <= 400)
            mode |= 0x40;
        else if (width <= 560)
            mode |= 2;
        else
            mode |= 3;
        {
            int h = e->disp.h;
            int small;
            if (e->pad0)
                small = h <= 288;
            else
                small = h <= 256;
            if (!small)
                mode |= 0x24;
        }
        D_80095744->callback10(mode);
        e->pad0 = 8;
    }
    rect = D_800957C0;
    asm("" : "=r"(rect) : "0"(rect));
    if (SIGNED_HALF(rect) != e->screen.x || SIGNED_HALF(rect + 1) != e->screen.y ||
        SIGNED_HALF(rect + 2) != e->screen.w || SIGNED_HALF(rect + 3) != e->screen.h ||
        e->pad0 == 8) {
        e->pad0 = GetVideoMode();
        {
            register int y asm("$4") = e->screen.y;
            vstart = e->pad0 ? y + 19 : y + 16;
        }
        vend = vstart + (e->screen.h ? e->screen.h : 240);
        width = e->disp.w;
        index = 0;
        if (width > 280) {
            index = 1;
            if (width > 352) {
                index = 2;
                if (width > 400) {
                    index = 4;
                    if (width <= 560)
                        index = 3;
                }
            }
        }
        {
            int product = e->screen.x * D_80095848[index];
            int video = e->pad0;
            int table = (video * 5 + index) * 2;
            register int base = D_80095820[table];
            register int screenWidth = e->screen.w;
            span = D_80095822[table] - base;
            hstart = base + product;
            if (screenWidth)
                span = (span * screenWidth) >> 8;
        }
        hend = hstart + span;
        asm volatile("" : : "r"(hend) : "memory");
        if (e->pad0) {
            clamp = 540;
            if (hstart >= 540) {
                clamp = 3220;
                if (hstart <= 3220)
                    clamp = hstart;
            }
            hstart = clamp;
            clamp = hstart + 4 * D_80095848[index];
            if (hend >= clamp)
                clamp = hend > 3260 ? 3260 : hend;
            hend = clamp;
            clamp = vstart < 19 ? 19 : vstart > 303 ? 303 : vstart;
            vstart = clamp;
            clamp = vstart + 2;
            if (vend >= clamp)
                clamp = vend > 305 ? 305 : vend;
        } else {
            clamp = 500;
            if (hstart >= 500) {
                clamp = 3250;
                if (hstart <= 3250)
                    clamp = hstart;
            }
            hstart = clamp;
            clamp = hstart + 4 * D_80095848[index];
            if (hend >= clamp)
                clamp = hend > 3290 ? 3290 : hend;
            hend = clamp;
            clamp = vstart < 16 ? 16 : vstart > 257 ? 257 : vstart;
            vstart = clamp;
            clamp = vstart + 2;
            if (vend >= clamp)
                clamp = vend > 258 ? 258 : vend;
        }
        vend = clamp;
        {
            register unsigned y asm("$3") = ((unsigned)hend & 0xfff) << 12;
            register unsigned x asm("$4") = ((unsigned)hstart & 0xfff);
            register unsigned opcode = 0x06000000;
            register GpuCallbacks *ops asm("$5") = D_80095744;
            x |= opcode;
            ops->callback10(y | x);
        }
        {
            register unsigned y asm("$3") = ((unsigned)vend & 0x3ff) << 10;
            register unsigned x asm("$4") = ((unsigned)vstart & 0x3ff);
            register unsigned opcode = 0x07000000;
            register GpuCallbacks *ops asm("$5") = D_80095744;
            x |= opcode;
            ops->callback10(y | x);
        }
    }
    /* An unprototyped call keeps the SDK memcpy call instead of builtin expansion.
     * Pointers and the word-sized count keep the same argument ABI. */
    ((void *(*)())memcpy)(&D_800957B8, e, 20);
    return e;
}
