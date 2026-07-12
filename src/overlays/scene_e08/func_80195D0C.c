typedef signed short s16;
typedef unsigned char u8;

typedef struct {
    char pad_00[0x28];
    int x;
    int y;
    char pad_30[8];
    s16 field_38;
    s16 angle;
    char pad_3C[4];
    int field_40;
} Ovl178Actor;

typedef struct {
    char pad_00[0x28];
    int x;
    char pad_2C[4];
    int z;
} Ovl178Linked;

typedef struct {
    char pad_00[0x60];
    int target_x;
    char pad_64[4];
    int target_z;
    char pad_6C[0x18];
    Ovl178Linked *linked;
    char pad_88[0x10];
    int distance;
    char pad_9C[6];
    s16 turn_step;
    s16 idle_step;
    char pad_A6[3];
    u8 clamp_on_distance;
} Ovl178TurnState;

int FieldEng_VecToAngle(void *from, void *to);
int FieldEng_TurnToward(int current, int target, int step);
void func_800DFE94(void *pos, void *field_40, void *field_38);

void func_80195D0C(Ovl178Actor *actor, Ovl178TurnState *state, void *arg2) {
    register Ovl178TurnState *state_reg __asm__("s0");
    register Ovl178Actor *actor_reg __asm__("s1");
    register int over_limit __asm__("v0");
    register int distance __asm__("v1");
    Ovl178Linked *linked;
    int angle;

    state_reg = state;
    actor_reg = actor;
    if (state_reg->turn_step > 0) {
        linked = state_reg->linked;
        if (linked != 0) {
            state_reg->target_x = linked->x;
            state_reg->target_z = linked->z;
        }

        angle = FieldEng_VecToAngle(&state_reg->target_x, &actor_reg->x);
        angle = FieldEng_TurnToward(actor_reg->angle, (s16)angle, state_reg->turn_step);
        actor_reg->angle = angle & 0xFFF;

        if (state_reg->clamp_on_distance != 0) {
            over_limit = 0x7FFFFFF;
            distance = state_reg->distance;
            over_limit = over_limit < distance;
            if (over_limit != 0) {
                state_reg->turn_step = 0;
                state_reg->idle_step = 0;
                angle = over_limit;
                goto set_angle;
            }
        }
    } else {
        func_800DFE94(&actor_reg->x, &actor_reg->field_40, &actor_reg->field_38);
        angle = FieldEng_TurnToward(actor_reg->angle, actor_reg->angle, state_reg->idle_step);
set_angle:
        actor_reg->angle = angle & 0xFFF;
    }
}
