typedef signed char s8;
typedef unsigned char u8;

typedef struct {
    char pad0[0xE];
    u8 selector;
} Ovl178Target;

typedef struct {
    char pad0[0x8];
    Ovl178Target *target;
    void (*handler)(void);
    char pad10[0x6];
    s8 value16;
} Ovl178State;

extern void Ovl178_IdleHandler_801983BC(void) __asm__(".L801983BC");

void func_80198394(Ovl178State *state) {
    if (state->target->selector == state->value16) {
        state->handler = Ovl178_IdleHandler_801983BC;
    }
}
