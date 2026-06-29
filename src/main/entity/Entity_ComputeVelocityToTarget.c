typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    char pad00[0x28];
    int field28;
    int field2C;
    int field30;
    char pad34[0x34];
    int field68;
    int field6C;
    int field70;
    char pad74[0x18];
    int field8C;
    char pad90[8];
    int flags98;
} State;

extern State *g_CurrentEntity;

int Math_FixedDivide(int arg0, int arg1);
int Math_FixedMul(int arg0, int arg1);

int Entity_ComputeVelocityToTarget(int **arg0) {
    State *state;
    int delta28;
    int delta2C;
    int delta30;
    int scale;
    int temp;

    state = g_CurrentEntity;
    state->flags98 |= 2;

    delta28 = state->field28 - *arg0[0];
    delta2C = state->field2C - *arg0[1];
    delta30 = state->field30 - *arg0[2];
    scale = *arg0[3] << 16;

    g_CurrentEntity->field68 = Math_FixedDivide(delta28, scale);
    g_CurrentEntity->field70 = Math_FixedDivide(delta30, scale);
    temp = Math_FixedMul(scale + 0x10000, g_CurrentEntity->field8C) >> 1;
    g_CurrentEntity->field6C = temp;
    g_CurrentEntity->field6C = Math_FixedDivide(delta2C, scale) + g_CurrentEntity->field6C;

    return 1;
}
