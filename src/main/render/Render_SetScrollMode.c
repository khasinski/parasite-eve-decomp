/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef short s16;

extern struct { char _[16]; } D_800BCF88_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCF8C_o __asm__("D_800BCF8C");
extern struct { char _[16]; } D_800BCF98_o __asm__("D_800BCF98");
extern struct { char _[16]; } D_800BCF9C_o __asm__("D_800BCF9C");
extern struct { char _[16]; } D_800BCF9E_o __asm__("D_800BCF9E");
extern struct { char _[16]; } D_800BCFA0_o __asm__("D_800BCFA0");
extern struct { char _[16]; } D_800BCFA2_o __asm__("D_800BCFA2");

#define D_800BCF88 (*(int *)&D_800BCF88_o)
#define D_800BCF8C (*(int *)&D_800BCF8C_o)
#define D_800BCF98 (*(int *)&D_800BCF98_o)
#define D_800BCF9C (*(s16 *)&D_800BCF9C_o)
#define D_800BCF9E (*(s16 *)&D_800BCF9E_o)
#define D_800BCFA0 (*(s16 *)&D_800BCFA0_o)
#define D_800BCFA2 (*(s16 *)&D_800BCFA2_o)

int Render_SetScrollMode(int x, int y, int z, int mode) {
    register int *flags_ptr asm("$9");
    register int flags asm("$8");
    register int value asm("$2");
    register int saved_scroll asm("$3");

    flags_ptr = &D_800BCF88;
    flags = *flags_ptr;
    if ((flags & 0x40) == 0) {
        return -0x13;
    }

    value = 1;
    saved_scroll = D_800BCF8C;
    D_800BCFA0 = value;
    value = -0x10;
    D_800BCF9C = x;
    D_800BCF9E = y;
    D_800BCFA2 = z;
    D_800BCF98 = saved_scroll;


    saved_scroll = flags & value;
    value = 8;
    if (mode != value) {
        value = saved_scroll | 1;
    } else {
        value = saved_scroll | 9;
    }
    *flags_ptr = value;

    return 0;
}
