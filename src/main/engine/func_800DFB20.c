typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    char pad0[0x8];
    void *target;
    char padC[0x8];
    s16 counter;
    char pad16[0x2];
    s8 value;
} Func800DFB20State;

typedef struct {
    char pad0[0xF];
    u8 limit;
    char pad10[0x4];
    int value;
    char pad18[0x2];
    u16 cursor;
} Func800DFB20Target;

void func_800DFB20(Func800DFB20State *state)
{
    Func800DFB20Target *target = state->target;

    if (state->counter != 0 && target->cursor >= target->limit) {
        target->value = state->value;
        if (state->counter != -1) {
            state->counter--;
        }
    }
}
