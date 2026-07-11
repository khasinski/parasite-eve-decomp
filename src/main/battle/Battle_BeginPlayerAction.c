typedef signed short s16;

extern int g_FieldMoveLock __asm__("D_8009D2E8");
extern char *g_PlayerEntity __asm__("D_8009D254");
extern char *g_ActiveActor __asm__("D_8009D278");

void Entity_SetActionMode(char *arg0, int arg1);
void Battle_FlushScriptSounds(void);

void Battle_BeginPlayerAction(void) {
    char *player;
    register char *actor asm("$5");
    register int mask asm("$3");
    register int flags asm("$2");

    mask = -0x101;
    g_FieldMoveLock |= 1;
    player = g_PlayerEntity;
    flags = *(int *)(player + 0x98);
    actor = g_ActiveActor;
    flags &= mask;
    *(int *)(player + 0x98) = flags;
    flags = *(int *)(actor + 0x4C);
    flags |= 0x10000;
    *(int *)(actor + 0x4C) = flags;
    asm("sh $0,%gp_rel(D_8009D298)($28)");
    Entity_SetActionMode(player, 0x12);
    Battle_FlushScriptSounds();
}
