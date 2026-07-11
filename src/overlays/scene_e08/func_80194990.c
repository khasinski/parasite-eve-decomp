typedef signed short s16;
typedef unsigned char u8;

void *func_800C2B50(void);

typedef struct {
    int x;
    int y;
    int z;
    char pad_0C[4];
    s16 field_10;
    s16 field_12;
    s16 field_14;
    char pad_16[6];
    s16 field_1C;
    s16 field_1E;
    s16 field_20;
    u8 field_22;
} Ovl178MotionState;

typedef struct {
    char pad_00[0x18];
    int x;
    int y;
    int z;
} Ovl178Position;

void func_80194990(void *arg0, void *arg1, Ovl178MotionState *state) {
    register Ovl178MotionState *state_reg __asm__("s0");
    Ovl178Position *position;
    int z;

    state_reg = state;
    position = func_800C2B50();

    state_reg->x = position->x - 0xC8;
    state_reg->y = position->y;
    z = position->z;
    state_reg->field_10 = 0;
    state_reg->field_12 = 0;
    state_reg->field_14 = 0;
    state_reg->field_1C = 0xC04;
    state_reg->field_22 = 0;
    state_reg->field_1E = 0;
    state_reg->field_20 = 0;
    state_reg->z = z;
}
