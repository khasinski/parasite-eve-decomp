typedef struct RoomDriftView {
    char pad0[0x1A0];
    char transform[0x14];
    s32 base_x;
    s32 base_y;
    s32 base_z;
} RoomDriftView;

typedef struct RoomDriftRoot {
    char pad0[0x238];
    RoomDriftView *view;
} RoomDriftRoot;

typedef struct RoomDriftEffect {
    s16 x;
    s16 y;
    s16 z;
    char pad6[0x2];
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    char padE[0x2];
    s16 timer;
    s16 duration;
    s16 half14;
} RoomDriftEffect;

typedef struct RoomDriftMatrixSeed {
    char bytes[8];
} RoomDriftMatrixSeed;

RoomDriftRoot **func_800C2B50(void);
void ApplyMatrixSV(void *transform, RoomDriftMatrixSeed *seed, u16 *out);
s32 rand(void);

extern RoomDriftMatrixSeed D_8018F01C;

void func_801929B0(void *arg0, void *arg1, RoomDriftEffect *effect) {
    register RoomDriftEffect *initial asm("$16");
    register RoomDriftView *view asm("$18");
    register RoomDriftEffect *current asm("$17");
    RoomDriftMatrixSeed seed;
    u16 out[4];
    register s32 random asm("$2");
    u16 timer;
    u16 x;
    u16 y;
    u16 z;
    u16 velocity_x;
    u16 velocity_y;
    u16 velocity_z;

    asm("" : "=r"(initial) : "0"(effect));
    view = (*func_800C2B50())->view;
    seed = D_8018F01C;
    ApplyMatrixSV(&view->transform, &seed, out);

    current = initial;
    if (initial->timer == 0) {
        initial->half14 = 0;
        initial->x = out[0] + view->base_x;
        initial->y = out[1] + view->base_y + 0x1E;
        initial->z = out[2] + view->base_z;
        initial->velocity_x = 0;
        initial->velocity_y = -8;
        initial->velocity_z = 0;
        random = rand();
        initial->duration = 0x96;
        initial->timer = random % 88 + 40;
    }

    x = current->x;
    velocity_x = current->velocity_x;
    velocity_y = current->velocity_y;
    velocity_z = current->velocity_z;
    current->x = x + velocity_x;
    y = current->y;
    z = current->z;
    current->y = y + velocity_y;
    current->z = z + velocity_z;
    random = rand();
    timer = current->timer - 4;
    current->timer = timer;
    current->duration += random % 30 + 50;
    if ((s16)timer < 0) {
        current->timer = 0;
    }
}
