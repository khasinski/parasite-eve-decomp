typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))

extern char *g_CurrentEntity;
extern char *D_8009D254;
extern char D_800B0CEC[];
extern u16 D_800B0D88;
extern u32 D_8009D2E8;
extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

void Anim_SetInterpRate(void *anim, int rate);

static void Task_PlayEntityAnimYield(void) {
    g_SceneDataTable0 -= 0x0C;
    U32_AT(g_TaskNodePool, 0x10) = 1;
}

int Task_PlayEntityAnim(int **args) {
    char *task;
    char *entity;
    int rate;
    u16 flags;

    task = g_TaskNodePool;
    entity = g_CurrentEntity;
    flags = U16_AT(task, 8);

    if ((flags & 0x20) != 0) {
        if ((U16_AT(entity, 0x250) & 4) != 0) {
            Task_PlayEntityAnimYield();
            return 0;
        }

        U16_AT(task, 8) = flags & 0xFFDF;
        return 1;
    }

    U32_AT(entity, 0x98) &= ~0x40;
    U16_AT(task, 8) = flags | 0x20;

    rate = (s16)*args[0];
    Anim_SetInterpRate(entity + 0x1B4, rate);
    U16_AT(entity, 0x250) |= 4;

    if (entity == D_8009D254) {
        D_8009D2E8 &= ~2;
        Anim_SetInterpRate(D_800B0CEC, rate);
        D_800B0D88 |= 4;
    }

    Task_PlayEntityAnimYield();
    return 0;
}
