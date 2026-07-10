/* CC1_FLAGS: -O1 */
typedef signed short s16;
typedef unsigned short u16;

extern s16 D_80095750;
extern s16 D_80095752;

int Gpu_BuildDrawAreaBottomRightCmd(int x, int y) {
    register int clamped_x asm("$2");
    register int signed_coord asm("$6");
    register int packed_y asm("$3");
    register int x_mask asm("$2");
    register int cmd_base asm("$4");
    int limit;
    int exceeds_limit;

    asm volatile("sll $2,%1,16\n\tsra %0,$2,16" : "=r"(signed_coord) : "r"(x) : "$2");
    clamped_x = 0;
    if (signed_coord >= 0) {
        exceeds_limit = D_80095750 - 1 < signed_coord;
        limit = (u16)D_80095750;
        if (exceeds_limit) {
            clamped_x = limit - 1;
        } else {
            clamped_x = x;
        }
    }

    x = clamped_x;
    asm volatile("sll $2,%1,16\n\tsra %0,$2,16" : "=r"(signed_coord) : "r"(y) : "$2");
    if (signed_coord >= 0) {
        exceeds_limit = D_80095752 - 1 < signed_coord;
        limit = (u16)D_80095752;
        if (exceeds_limit) {
            y = limit - 1;
            goto y_clamped;
        }
        packed_y = y & 0x3FF;
        goto y_done;
    }
    y = 0;

y_clamped:
    packed_y = y & 0x3FF;

y_done:
    packed_y <<= 10;
    x_mask = x & 0x3FF;
    cmd_base = 0xE4000000;
    x_mask |= cmd_base;
    x_mask = packed_y | x_mask;
    return x_mask;
}
