#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Obj_GetEntryField6(int arg0);

void Obj_SetEntryField8(int arg0, int arg1);

void Obj_FillEntrySlotValues(int arg0, int arg1);

void Obj_SetEntrySlotValue(int arg0, int arg1, int arg2);

void Obj_SetEntryFlags(int arg0, int arg1);

extern int g_CollisionPlaneTable;
extern char *g_CollisionDb;

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
        char *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 11);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 7);
        base_entry = *(char **)(base + 0x1C);
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
        char *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 11);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        asm volatile("" : "=r"(index) : "0"(index));
        base = g_CollisionDb;
        asm volatile("" : "=r"(base) : "0"(base));
        entry = (char *)(index * 7);
        base_entry = *(char **)(base + 0x1C);
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
#include "pe1/field_actor.h"

int Gpu_LoadGeomState(int index);

extern short g_CameraBaseAngleX;
extern short g_CameraBaseAngleY;

int CdRom_SetSeekPos(unsigned int arg0);

int Render_SetFadeColour(unsigned int arg0);

int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, unsigned short arg4);

extern unsigned char g_ScreenTransitionState;

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_PlayerEntity;
extern short D_800BCFFE;

int Task_LoadGeomState(int **arg0) {
    Gpu_LoadGeomState(**arg0);
    return 1;
}

int Task_SetScreenScrollPos(int **arg0) {
    g_CameraBaseAngleX = *arg0[0];
    asm volatile("" ::: "memory");
    g_CameraBaseAngleY = *arg0[1];
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
