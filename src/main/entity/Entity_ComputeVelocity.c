typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    char pad00[0x20];
    int field20;
    u16 field24;
    u16 field26;
    char pad28[0x12];
    s16 field3A;
    char pad3C[0x2C];
    int field68;
    char pad6C[4];
    int field70;
} State;

extern State *g_CurrentEntity;
extern State *g_PlayerEntity;

int Math_FixedMul(int arg0, int arg1);
int rsin(int arg0);
int rcos(int arg0);

int Entity_ComputeVelocity(void) {
    State *state = g_CurrentEntity;
    int temp;

    if (state == g_PlayerEntity) {
        temp = Math_FixedMul(0x50000, state->field20);
    } else {
        temp = state->field20;
    }

    temp = Math_FixedMul(temp, g_CurrentEntity->field26 << 4);
    g_CurrentEntity->field68 = Math_FixedMul(-temp, rsin(g_CurrentEntity->field3A) << 4);
    g_CurrentEntity->field70 = Math_FixedMul(-temp, rcos(g_CurrentEntity->field3A) << 4);

    return 1;
}
