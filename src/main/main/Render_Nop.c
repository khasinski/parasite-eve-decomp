/* MASPSX_FLAGS: --store-branch-delay */

typedef unsigned char u8;
typedef unsigned short u16;

extern int D_800BE9EC;
extern int D_800C0DBC;
extern int D_800C20C4;
extern u8 *D_800C0DC8;

int Render_Nop(void **out0, void **out1) {
    register void **out0_reg asm("$7");
    register void **out1_reg asm("$8");
    u8 *entry;
    int index;

    out0_reg = out0;
    index = D_800BE9EC;
    entry = D_800C0DC8 + (index << 5);

    out1_reg = out1;
    if (*(u16 *)entry == 1) {
        D_800BE9EC = 0;
        if (D_800C0DBC != 0) {
            *(u16 *)entry = 0;
        }
        index = D_800BE9EC;
        entry = D_800C0DC8 + (index << 5);
    }

    asm volatile("" : : : "memory");
    if (*(u16 *)entry == 2) {
        *(u16 *)entry = 4;
        *out0_reg = D_800C0DC8 + (D_800C20C4 << 5) + (((D_800BE9EC << 6) - D_800BE9EC) << 5);
        *out1_reg = entry;
        return 0;
    }
    return 1;
}
