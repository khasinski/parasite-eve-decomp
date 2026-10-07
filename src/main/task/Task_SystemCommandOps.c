/* Script opcodes from 0x80018660 to 0x80018FDC: combat timer and mode flags,
 * process-manager commands, actor animation and collision helpers, object
 * entry setters, camera/CD/fog settings and actor move speed. All are
 * default-profile handlers that reach the current actor through absolute
 * addresses. */
#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_PlayerEntity;
extern int g_CombatModeFlags;
extern int D_800A76C8;
extern int D_800A76CC;
extern char * volatile g_GeomState;
extern int g_CollisionPlaneTable;
extern CollisionDatabase *g_CollisionDb;
/* Camera base angles at 0x800BD020. As a record, the second argument load
 * stays behind the first store: GCC 2.7 lets a varying array load pass a
 * store to a fixed scalar, but not to a structure member. */
typedef struct CameraBaseAngles {
    short x;
    short y;
} CameraBaseAngles;

extern CameraBaseAngles D_800BD020;
extern unsigned char g_ScreenTransitionState;
extern short D_800BCFFE;

int Scene_LoadRoomAssets(int arg0, void *arg1);
int Pm_SendCmd();
int Pm_SetGetState(int arg0, int arg1, int arg2);
void Entity_AllocSlot(void *arg0);
void Task_SetObjAnimEntry12(
    void *arg0,
    int arg1,
    int arg2,
    int arg3,
    int arg4,
    int arg5,
    int arg6,
    int arg7,
    int arg8,
    int arg9,
    int arg10,
    int arg11);
void Task_SetObjAnimEntry5(void *arg0, int arg1, int arg2, int arg3, int arg4, int arg5);
void Geo_TransformPoint(void *arg0, int arg1, int arg2, int arg3);
void Render_SetEntryVisible(int arg0, int arg1);
void Geo_ClipPoint(int arg0, int arg1, int arg2);
void Obj_SetEntryLimit(int arg0, int arg1);
void Sys_SetFlagSlot(int arg0, int arg1);
int Obj_GetEntryField6(int arg0);
void Obj_SetEntryField8(int arg0, int arg1);
void Obj_FillEntrySlotValues(int arg0, int arg1);
void Obj_SetEntrySlotValue(int arg0, int arg1, int arg2);
void Obj_SetEntryFlags(int arg0, int arg1);
int Gpu_LoadGeomState(int index);
int CdRom_SetSeekPos(unsigned int arg0);
int Render_SetFadeColour(unsigned int arg0);
int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, unsigned short arg4);

/* Arms the combat countdown: args are two minute-or-hour terms summed and
 * scaled to 60 Hz frames (216000 per unit), plus seconds (60 frames each). */
int Sys_ComputeAudioTimer(int **arg0) {
    int *dst;
    int value;

    dst = &D_800A76C8;
    value = (*arg0[0] + *arg0[1]) * 216000;
    value += *arg0[2] * 60;
    *dst = value;
    if (*arg0[3] == 1) {
        D_800A76CC = 0;
        g_CombatModeFlags |= 2;
    } else {
        *dst = 0;
    }
    {
        int *flags = &g_CombatModeFlags;

        *flags = (*flags | 1) & ~4;
    }
    return 1;
}

int Task_GetCombatModeFlag(int **arg0) {
    if (g_CombatModeFlags & 4) {
        *arg0[0] = 1;
    } else {
        *arg0[0] = 0;
    }

    return 1;
}

int Task_SetCombatModeFlag(void) {
    int *ptr = &g_CombatModeFlags;

    *ptr |= 4;
    return 1;
}

int Task_GetEntityEffect(int **arg0) {
    int value;

    value = Scene_LoadRoomAssets(*arg0[0], g_CurrentEntity);
    *arg0[1] = value;
    return 1;
}

typedef struct WrapperArgs {
    int *arg0;
    int *arg2;
    int *arg3;
    int *arg4;
    int *arg5;
} WrapperArgs;

int Pm_ScriptSendCommand0(WrapperArgs *args) {
    int *ptr0;
    int *ptr2;
    int *ptr3;
    int *ptr4;
    int value4;
    int value0;
    int value2;

    ptr0 = args->arg0;
    ptr4 = args->arg4;
    ptr2 = args->arg2;
    value4 = *ptr4;
    value0 = *ptr0;
    value2 = *ptr2;
    ptr3 = args->arg3;
    Pm_SendCmd(value0, 0, value2, *(volatile int *)ptr3, value4, *(volatile int *)args->arg5);
    return 1;
}

int Pm_ScriptSendCommand1(int **arg0) {
    int **base;
    register int *op0;
    register int *op1;
    int *stack0;
    register int first;
    int second;

    base = arg0;
    op0 = base[0];
    op1 = base[1];
    stack0 = base[3];
    first = *op0;
    second = *op1;

    Pm_SendCmd(first, 1, second, base[2], stack0, base[4]);
    return 1;
}

int Pm_ScriptSetState(int **arg0) {
    Pm_SetGetState(*arg0[0], 0, *arg0[1]);
    return 1;
}

int Pm_ScriptGetState(int **arg0) {
    Pm_SetGetState(*arg0[0], 1, arg0[1]);
    return 1;
}

int Task_PlayerPointInPoly(int **arg0) {
    int values[8];
    unsigned int i;

    for (i = 0; i < 4; i++) {
        values[i * 2] = *arg0[i * 2];
        values[i * 2 + 1] = *arg0[i * 2 + 1];
    }

    *arg0[8] = Geo_PointInPoly(
        g_CurrentEntity->pos_x,
        g_CurrentEntity->pos_z,
        (const PolygonVertex *)values,
        4);
    return 1;
}

int Task_InitEntityMoveState(void) {
    Entity_AllocSlot(g_CurrentEntity);
    return 1;
}

int Task_SetEntityAnim12Args(char **arg0) {
    Task_SetObjAnimEntry12(
        g_CurrentEntity,
        *(unsigned char *)arg0[0],
        *(unsigned char *)arg0[1],
        *(unsigned char *)arg0[2],
        *(unsigned char *)arg0[3],
        *(unsigned short *)arg0[4],
        *(signed char *)arg0[5],
        *(signed char *)arg0[6],
        *(signed char *)arg0[7],
        *(signed char *)arg0[8],
        *(unsigned char *)arg0[9],
        *(unsigned char *)arg0[10]);
    return 1;
}

int Task_SetEntityAnim5Args(char **arg0) {
    Task_SetObjAnimEntry5(
        g_CurrentEntity,
        *(unsigned char *)arg0[0],
        *(unsigned char *)arg0[1],
        *(unsigned char *)arg0[2],
        *(unsigned char *)arg0[3],
        *(unsigned short *)arg0[4]);
    return 1;
}

int Task_SetSceneEntryAnim(int **arg0) {
    char *header = g_GeomState;
    char *base = g_GeomState;
    int index = *arg0[0];

    Geo_TransformPoint(
        base + *(int *)(header + 0x14) + (index * 56),
        *(short *)arg0[1],
        *(short *)arg0[2],
        *(short *)arg0[3]);
    return 1;
}

int Task_SetRenderEntryEnabled(int **arg0) {
    Render_SetEntryVisible(*arg0[0], *arg0[1]);
    return 1;
}

int Task_SetRenderEntryPos(short **arg0) {
    Geo_ClipPoint(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}

int Task_SetObjEntryLimit(int **arg0) {
    Obj_SetEntryLimit(*arg0[0], *arg0[1]);
    return 1;
}

int Task_SetFlagSlot(int **arg0) {
    Sys_SetFlagSlot(*arg0[0], *arg0[1]);
    return 1;
}

int Entity_ClearCurrentFlag20(void) {
    g_CurrentEntity->flags &= -0x21;
    return 1;
}

int Entity_SetCurrentFlag20(void) {
    g_CurrentEntity->flags |= 0x20;
    return 1;
}

int Task_GetObjEntryField6(int **arg0) {
    *arg0[1] = Obj_GetEntryField6(*arg0[0]);
    return 1;
}

int Task_SetObjEntryField8(int **arg0) {
    Obj_SetEntryField8(*arg0[0], *arg0[1]);
    return 1;
}

int Task_FillObjEntrySlotValues(int **arg0) {
    Obj_FillEntrySlotValues(*arg0[0], *arg0[1]);
    return 1;
}

int Task_SetObjEntrySlotValue(int **arg0) {
    Obj_SetEntrySlotValue(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}

int Task_SetObjEntryFlags(int **arg0) {
    Obj_SetEntryFlags(*arg0[0], *arg0[1]);
    return 1;
}

int Task_ClearObjEntryFlags(int **arg0) {
    Obj_SetEntryFlags(*arg0[0], ~*arg0[1]);
    return 1;
}

int Task_SetObjEntryFlag80(int **arg0) {
    register char *entry asm("$3");
    char *base_entry;
    int value;
    if (g_CollisionPlaneTable == 0) {
        int index;
        CollisionDatabase *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 11);
        base_entry = base->triangles.pointer;
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        CollisionDatabase *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 7);
        base_entry = base->triangles.pointer;
        /* Keep the base load branch-local without materializing the add before the join. */
        asm volatile("" : : "r"(base_entry));
        entry = (char *)((int)entry << 2);
    }
    entry = (char *)((int)entry + (int)base_entry);
        value = *entry | 0x80;
    *entry = value;
    asm volatile("" : : : "memory");
    return 1;
}

int Task_ClearObjEntryFlag80(int **arg0) {
    register char *entry asm("$3");
    char *base_entry;
    int value;
    if (g_CollisionPlaneTable == 0) {
        int index;
        CollisionDatabase *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 11);
        base_entry = base->triangles.pointer;
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        CollisionDatabase *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 7);
        base_entry = base->triangles.pointer;
        /* Keep the base load branch-local without materializing the add before the join. */
        asm volatile("" : : "r"(base_entry));
        entry = (char *)((int)entry << 2);
    }
    entry = (char *)((int)entry + (int)base_entry);
        value = *entry & 0x7F;
    *entry = value;
    asm volatile("" : : : "memory");
    return 1;
}

int Task_LoadGeomState(int **arg0) {
    Gpu_LoadGeomState(**arg0);
    return 1;
}

int Task_SetScreenScrollPos(int **arg0) {
    D_800BD020.x = *arg0[0];
    D_800BD020.y = *arg0[1];
    return 1;
}

int Task_SetCdRomSeekPos(unsigned short **arg0) {
    CdRom_SetSeekPos(**arg0);
    return 1;
}

int Task_SetFogColor(unsigned short **arg0) {
    Render_SetFadeColour(**arg0);
    return 1;
}

int Task_SetCdRomScreenPos(unsigned short **arg0) {
    return CdRom_SetScreenPos(*arg0[0], *arg0[1], *arg0[2], *arg0[3], *arg0[4]), 1;
}

int Task_ClearEntityFlag40(void) {
    unsigned char *ptr = &g_ScreenTransitionState;

    *ptr &= 0xBF;
    return 1;
}

int Task_SetEntityMoveSpeed(int **arg0) {
    int **args = arg0;
    FieldActor *current = g_CurrentEntity;
    FieldActor *selected = g_PlayerEntity;
    int value = *args[0] >> 4;

    current->move_speed = value;
    if (current == selected) {
        int scaled;

        scaled = ((*args[0] >> 4) * 3) << 7;

        if (scaled < 0) {
            scaled += 0xFFF;
        }
        D_800BCFFE = scaled >> 12;
    }
    return 1;
}
