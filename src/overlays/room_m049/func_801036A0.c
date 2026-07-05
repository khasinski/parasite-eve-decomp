typedef signed short s16;
typedef unsigned short u16;
typedef int s32;

typedef struct {
    char pad0[0x1A0];
    char transform[0x14];
    s32 base_x;
    s32 base_y;
    s32 base_z;
} Overlay024View;

typedef struct {
    char pad0[0x238];
    Overlay024View *view;
} Overlay024Root;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    char pad6[0x2];
    s16 half8;
    s16 velocity_y;
    s16 halfC;
    char padE[0x2];
    s16 timer;
    s16 duration;
    s16 half14;
} Overlay024Effect;

typedef struct {
    char bytes[8];
} Overlay024MatrixSeed;

Overlay024Root **func_800C2B50(void);
void func_80078C34(void *transform, Overlay024MatrixSeed *seed, u16 *out);
s32 func_80071A54(void);

extern Overlay024MatrixSeed D_8018F01C;

void func_801036A0(void *arg0, void *arg1, Overlay024Effect *effect) {
    Overlay024View *view;
    Overlay024MatrixSeed seed;
    u16 out[4];
    s32 random;

    view = (*func_800C2B50())->view;
    seed = D_8018F01C;
    func_80078C34(&view->transform, &seed, out);

    if (effect->timer == 0) {
        effect->half14 = 0;
        effect->x = out[0] + view->base_x;
        effect->y = out[1] + view->base_y;
        effect->z = out[2] + view->base_z;
        effect->half8 = 0;
        effect->velocity_y = -8;
        effect->halfC = 0;
        random = func_80071A54();
        effect->duration = 0x96;
        effect->timer = random % 80 + 0x50;
    }
}
