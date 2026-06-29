/* CC1_FLAGS: -g3 */

typedef unsigned short u16;
typedef short s16;

struct Unk800B1624 {
    char pad[0x24];
    u16 field_24;
    u16 field_26;
    u16 field_28;
    u16 field_2A;
    u16 field_2C;
    u16 field_2E;
    s16 min_x;
    s16 max_x;
    s16 min_y;
    s16 max_y;
};

extern struct Unk800B1624 *g_GeomState;

#define MOVE_REG(dst, src) asm("addu %0,%1,$0" : "=r"(dst) : "r"(src))

int Geo_ClipPoint(int x, int y, int z) {
    struct Unk800B1624 *ptr = g_GeomState;
    register int x_value asm("$3");
    register int tmp asm("$2");
    int out_x;
    int out_y;
    register int z_value asm("$4");
    int limit;
    volatile int stack_pad[8];

    x_value = ptr->field_28 + x;
    MOVE_REG(out_x, x_value);
    tmp = ptr->field_2A;
    out_y = tmp + y;
    x_value = (s16)x_value;
    z_value = ptr->field_24;

    tmp = ptr->min_x;
    MOVE_REG(limit, tmp);
    z_value += z;
    if (x_value < tmp) {
        out_x = limit;
    } else {
        tmp = ptr->max_x;
        MOVE_REG(limit, tmp);
        if (tmp < x_value) {
            out_x = limit;
        }
    }

    x_value = out_y << 16;
    tmp = ptr->min_y;
    x_value = x_value >> 16;
    MOVE_REG(limit, tmp);
    if (x_value < tmp) {
        out_y = limit;
    } else {
        tmp = ptr->max_y;
        MOVE_REG(limit, tmp);
        if (tmp < x_value) {
            out_y = limit;
        }
    }

    ptr->field_2C = out_x;
    ptr->field_2E = out_y;
    ptr->field_26 = z_value;
    return 0;
}
