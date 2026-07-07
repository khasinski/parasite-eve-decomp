/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned short u16;

int g_FieldMoveLock;
void *g_EntityFreeListHead;
void *g_FieldActorListHead;
int g_CurrentEntity;
u16 g_EntityFreePoolCount;
void *g_PlayerEntity;
void *D_8009D224;

extern char g_EntityWorkBuffer[];
extern char g_TaskNodeActiveFlags[];
extern char D_800BEA90[];
extern unsigned int g_GameState[];

void Entity_ResetAllPools(void)
{
    unsigned int i;
    unsigned int row;
    char *ptr;
    unsigned int base;
    unsigned int col;
    register unsigned int offset asm("$3");

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
