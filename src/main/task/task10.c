/* CC1_FLAGS: -fno-schedule-insns */

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
    char *entry;
    char *base_entry;

    if (g_CollisionPlaneTable == 0) {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        base = g_CollisionDb;
        entry = (char *)(index * 11);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        base = g_CollisionDb;
        entry = (char *)(index * 7);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local without materializing the add before the join. */
        asm volatile("" : : "r"(base_entry));
        entry = (char *)((int)entry << 2);
    }
    /* GCC prints addu operands in RTL order; tie entry as operand 0 to match retail bytes. */
    asm volatile("addu %0,%0,%1" : "=r"(entry) : "r"(base_entry), "0"(entry));
    *entry |= 0x80;
    return 1;
}

int Task_ClearObjEntryFlag80(int **arg0) {
    char *entry;
    char *base_entry;

    if (g_CollisionPlaneTable == 0) {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        base = g_CollisionDb;
        entry = (char *)(index * 11);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        base = g_CollisionDb;
        entry = (char *)(index * 7);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local without materializing the add before the join. */
        asm volatile("" : : "r"(base_entry));
        entry = (char *)((int)entry << 2);
    }
    /* GCC prints addu operands in RTL order; tie entry as operand 0 to match retail bytes. */
    asm volatile("addu %0,%0,%1" : "=r"(entry) : "r"(base_entry), "0"(entry));
    *entry &= 0x7F;
    return 1;
}
