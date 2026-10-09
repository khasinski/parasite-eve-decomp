#include "common.h"
#include "pe1/entity_allocation.h"
#include "pe1/player_entity.h"
/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: -G8 --use-comm-section */

/* Entity pool reset and free-list construction share the pool globals and the
 * 14-entry 0x280-byte entity record block at D_800BEA90. */

int g_FieldMoveLock;
void *g_EntityFreeListHead;
void *g_FieldActorListHead;
int g_CurrentEntity;
u16 g_EntityFreePoolCount;
FieldActor *g_PlayerEntity;
void *D_8009D224;

extern char g_EntityWorkBuffer[];
extern char g_TaskNodeActiveFlags[];
extern char D_800BEA90[];
extern char D_800BEA94[];
extern int D_800C0B14[];
extern unsigned int g_GameState[];

void Entity_ResetAllPools(void)
{
    unsigned int i;
    unsigned int row;
    char *ptr;
    unsigned int base;
    unsigned int col;
    register unsigned int offset;

    g_FieldMoveLock = 0;

    i = 0;
    ptr = g_EntityWorkBuffer;
    do {
        *(int *)ptr = 0;
        i++;
        ptr += 4;
    } while (i < 0x200);

    i = 0;
    ptr = g_TaskNodeActiveFlags;
    do {
        i++;
        *(int *)ptr = 0;
    } while (i < 0x40);

    i = 0;
    base = (unsigned int)D_800BEA90;
    row = 0;
    do {
        col = 0;
        offset = row;
        do {
            *(int *)(offset + base) = 0;
            col++;
            offset += 4;
        } while (col < 0xA0);
        i++;
        row += 0x280;
    } while (i < 0xE);

    g_EntityFreeListHead = 0;
    g_FieldActorListHead = 0;
    g_CurrentEntity = 0;
    g_EntityFreePoolCount = 0;
    g_PlayerEntity = 0;
    D_8009D224 = 0;
    g_GameState[0] &= 0xFFFFCFFF;
}

void Entity_InitFreePool(void)
{
    int i;
    int offset;
    char *base;
    char *next;

    i = 0;
    base = D_800BEA90;
    next = base + 0x280;
    offset = 0;
    g_EntityFreePoolCount = 0;
    g_FieldActorListHead = 0;
    g_EntityFreeListHead = base;

    do {
        *(void **)(D_800BEA94 + offset) = next;
        next += 0x280;
        i++;
        offset += 0x280;
    } while ((unsigned int)i < 13);

    D_800C0B14[0] = 0;
    offset = 0;
    do {
        ((EntityAllocationBlock *)((u8 *)D_800A7620 + offset))->address = 0;
        offset += 8;
    } while ((unsigned int)offset < 0x80);

    g_PlayerEntity = 0;
}
