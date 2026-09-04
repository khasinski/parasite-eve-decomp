#include "../room_lib/room_lib.h"

typedef struct RoomRenderWords8 {
    int words[8];
} RoomRenderWords8;

extern unsigned char D_801971E8[];
extern unsigned char D_80197494;
extern unsigned char D_80197495;
extern unsigned char D_80197496;
extern unsigned char D_80197498;
extern unsigned char D_80197499;
extern unsigned char D_8019749A;
extern short D_8019749C;
extern short D_8019749E;
extern short D_801974A0;
extern short D_801974A2;
extern short D_801974A4;
extern unsigned char D_801974AC;
extern unsigned char D_801974AD;
extern unsigned char D_801974AE;
extern unsigned char D_801974B0;
extern unsigned char D_801974B1;
extern unsigned char D_801974B2;
extern short D_801974B4;
extern short D_801974B6;
extern short D_801974B8;
extern short D_801974BA;
extern short D_801974BC;
extern void func_800C4E50(void *params);

void func_80192A00(void *unused0, void *unused1, void *output_arg) {
    register char *work asm("$16") = output_arg;
    register short *anchor asm("$18");
    register int mode asm("$22");
    register int offset asm("$21");
    register int intensity asm("$20");
    register int primary_color asm("$17");
    register int blue asm("$23");
    register int extent asm("$19");
    register char *params asm("$4");
    register int white asm("$2");
    int primary_extent;
    char *root;

    root = (char *)func_800C2B50();
    PE1_COMPILER_LAUNDER(work);
    anchor = &D_8019749C;
    params = (char *)anchor - 12;
    mode = 0x10;
    offset = -500;
    intensity = 0x80;
    primary_color = 250;
    root = *(char **)root;
    blue = 120;
    root = *(char **)(root + 0x238);
    extent = 1400;
    *(RoomRenderWords8 *)work = *(RoomRenderWords8 *)(root + 0xA0);
    RW16(work, 0x30) = 1200;
    RW16(work, 0x34) = 255;

    primary_extent = 3200;
    PE1_COMPILER_USE(primary_extent);
    work = D_801971E8;
    *anchor = mode;
    D_801974A2 = offset;
    D_801974A4 = intensity;
    D_80197498 = primary_color;
    D_80197499 = primary_color;
    D_8019749A = blue;
    D_80197494 = 0;
    D_80197495 = 0;
    D_80197496 = 0;
    D_8019749E = primary_extent;
    D_801974A0 = extent;
    *(char **)params = work;
    func_800C4E50(params);

    params = (char *)anchor + 12;
    PE1_COMPILER_USE(params);
    white = 255;
    work += 0x100;
    D_801974B4 = mode;
    D_801974BA = offset;
    D_801974BC = intensity;
    D_801974B0 = white;
    D_801974B1 = white;
    D_801974B2 = white;
    D_801974AC = primary_color;
    D_801974AD = primary_color;
    D_801974AE = blue;
    D_801974B8 = 0;
    D_801974B6 = extent;
    *(char **)params = work;
    func_800C4E50(params);
}
