/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned short u16;

u16 g_EntityFreePoolCount;
void *g_FieldActorListHead;
void *g_EntityFreeListHead;
void *g_PlayerEntity;

extern char D_800BEA90[];
extern char D_800BEA94[];
extern int D_800C0B14[];
extern char g_EntityAllocBlockTable[];

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
        *(int *)(g_EntityAllocBlockTable + offset) = 0;
        offset += 8;
    } while ((unsigned int)offset < 0x80);

    g_PlayerEntity = 0;
}
