typedef unsigned char u8;
typedef unsigned int u32;

typedef struct Overlay174Flags {
    u32 flags;
} Overlay174Flags;

typedef struct Overlay174Holder {
    Overlay174Flags *target;
} Overlay174Holder;

typedef struct Overlay174State {
    u8 mode;
    char pad1[7];
    Overlay174Holder *holder;
} Overlay174State;

int func_800C6CE0(void);

int func_8018F190(Overlay174State *state) {
    state->mode = 4;
    if (func_800C6CE0() >= 2U) {
        state->holder->target->flags &= 0xC0FFFFFF;
    }
    return 0;
}
