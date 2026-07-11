typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct {
    char pad0[0xE];
    u8 mode;
    char padF[0x89];
    u32 flags;
} Actor;

typedef struct {
    s32 xy;
} TrigEntry;

typedef struct {
    s32 state;
} OverlayChildState;

typedef struct {
    s32 pad0;
    s32 pad4;
    char *position;
    void (*callback)(void *);
    OverlayChildState *child;
    char pad14[0x8];
    s16 facing_y;
    s16 facing_z;
    s16 vector_x;
    s16 vector_y;
    s16 scale_y;
    s16 vector_z;
    s16 neg_vector_x;
    s16 zero_2a;
    s16 facing_copy;
    char pad2E[0x16];
    s16 angle_a;
    char pad46[0x2];
    s16 angle_b;
} Overlay189FacingState;

extern Actor *D_8009D254;
extern TrigEntry D_800966EC[];
extern void D_801923A4(void *);

s16 func_800DFF80(void *arg0, void *arg1);

void func_801832A8(Overlay189FacingState *arg0) {
    Actor *actor;
    s16 angle;
    s32 facing_y;
    TrigEntry *entry;

    actor = D_8009D254;
    if (actor->mode < 4) {
        actor->flags &= 0xFFF3FFFF;
        arg0->callback = D_801923A4;

        if (arg0->child != 0) {
            arg0->child->state = 2;
        }

        if (arg0->angle_a == -1) {
            arg0->angle_a = func_800DFF80(arg0->position + 0x28, (char *)D_8009D254 + 0x28);
        }

        if (arg0->angle_b == -1) {
            arg0->angle_b = arg0->angle_a + 0x800;
        }

        angle = arg0->angle_a;
        entry = &D_800966EC[(u16)angle & 0xFFF];
        facing_y = *(s16 *)((char *)entry + 2);
        arg0->facing_y = facing_y;
        arg0->facing_z = 0;
        arg0->vector_x = entry->xy;
        arg0->vector_y = 0;
        arg0->vector_z = 0;
        arg0->scale_y = 0x1000;
        arg0->neg_vector_x = -arg0->vector_x;
        arg0->zero_2a = 0;
        arg0->facing_copy = arg0->facing_y;
    }
}
