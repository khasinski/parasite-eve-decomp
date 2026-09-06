typedef struct { int reserved[19]; unsigned int flags; } ActorFlags;
typedef struct {
    ActorFlags *actor;
    char reserved04[504];
    int x, y, z;
} WorldActor;
typedef struct { unsigned int *actor; } RoomActor;
extern WorldActor *g_PlayerEntity;
extern RoomActor **D_800F32D0;
extern short D_8019A86C, D_800942EC;
extern int Math_IntSqrt(int);

int func_8019721C(short *point, int radius) {
    short *gate = &D_8019A86C;
    int dx, dz;
    WorldActor *world;
    if (*gate != 0) return 0;
    if (point[5] < D_800942EC - 512) return 0;
    world = g_PlayerEntity;
    dx = world->x - point[4];
    if (dx < 0) dx = point[4] - world->x;
    if (dx > radius) return 0;
    dz = world->z - point[6];
    if (dz < 0) dz = point[6] - world->z;
    if (dz > radius) return 0;
    if (Math_IntSqrt(dx * dx + dz * dz) > radius) return 0;
    g_PlayerEntity->actor->flags |= 0x4000;
    if (D_800F32D0[2]->actor) *D_800F32D0[2]->actor |= 0x80000000U;
    *gate = 30;
    return 1;
}
