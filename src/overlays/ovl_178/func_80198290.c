typedef signed char s8;
typedef signed short s16;

typedef struct {
    char pad0[0xC];
    void (*handler)(void);
    void *signal;
    s16 timer;
    s8 value16;
    s8 value17;
    s8 value18;
    s8 mode;
    s8 flag1A;
    char pad1B[0x2B];
    s16 angle46;
} Ovl178State;

extern void Ovl178_IdleHandler_801983BC(void) __asm__(".L801983BC");

int func_80198290(Ovl178State *state) {
    state->value16 = -1;
    state->value17 = -1;
    state->value18 = -1;
    state->mode = 3;
    state->handler = Ovl178_IdleHandler_801983BC;

    state->signal = 0;
    state->timer = 0;
    state->flag1A = 0;
    state->angle46 = 0;

    return 0;
}
