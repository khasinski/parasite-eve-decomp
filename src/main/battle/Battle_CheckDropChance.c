typedef unsigned short u16;

extern void *g_ActiveActor;

int rand(void);

void Battle_CheckDropChance(void)
{
    void *ctx;
    int flags;
    u16 chance;

    ctx = g_ActiveActor;
    flags = *(int *)(*(char **)((char *)ctx + 0x68) + 0x10);
    if (flags & 0x10000) {
        chance = *(u16 *)((char *)ctx + 0x22);
        if ((rand() % 100) < chance) {
            *(short *)((char *)g_ActiveActor + 0x10) = 0x2328;
        }
    }
}
