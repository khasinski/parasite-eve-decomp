typedef signed char s8;
typedef signed short s16;
typedef int s32;

typedef struct Overlay189State Overlay189State;
typedef struct Overlay189Nested Overlay189Nested;

struct Overlay189State {
    s8 state;
    char pad1[0x2];
    s8 substate;
    char pad4[0x4];
    Overlay189Nested **nested_root;
    char padC[0x4];
    s32 *word_to_clear;
    s16 timer;
};

struct Overlay189Nested {
    char pad0[0x18];
    Overlay189State *state;
};

s32 func_801819F8(Overlay189State *arg0, s32 state) {
    Overlay189State **fields;

    state = 4;
    fields = (Overlay189State **)((char *)arg0 + 0xC);

    arg0->state = state;
    arg0->substate = 0;

    if (arg0->timer != 0 && arg0->nested_root[0] != 0) {
        arg0->nested_root[0]->state->state = state;
    }

    if (fields[1] != 0) {
        *(s32 *)fields[1] = 0;
    }

    return 0;
}
