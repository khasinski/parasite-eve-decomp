typedef unsigned char u8;
typedef unsigned int u32;

typedef struct Overlay175Flags {
    u32 flags;
} Overlay175Flags;

typedef struct Overlay175Holder {
    Overlay175Flags *target;
} Overlay175Holder;

typedef struct Overlay175State {
    u8 mode;
    char pad1[7];
    Overlay175Holder *holder;
} Overlay175State;

int func_800C6CE0(void);

int func_8018F190(Overlay175State *state) {
    state->mode = 4;
    if (func_800C6CE0() >= 2U) {
        state->holder->target->flags &= 0xC0FFFFFF;
    }
    return 0;
}
