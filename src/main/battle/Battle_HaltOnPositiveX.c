extern char *g_ActiveActor;
extern char *g_PlayerEntity;

void Entity_SetActionMode(char *arg0, int arg1);
void Battle_FlushScriptSounds(char *arg0);

void Battle_HaltOnPositiveX(void) {
    char *state;
    char *actor;
    char *state2;
    char *actor2;
    int flags;

    state = g_ActiveActor;
    if (*(short *)(state + 0xC) > 0) {
        actor = g_PlayerEntity;
        *(int *)(actor + 0x68) = 0;
        *(int *)(actor + 0x6C) = 0;
        *(int *)(actor + 0x70) = 0;
        Entity_SetActionMode(actor, 0x12);

        state2 = g_ActiveActor;
        actor2 = g_PlayerEntity;
        flags = *(int *)(state2 + 0x4C);
        asm volatile("sb $0, 0xC0($gp)");
        flags |= 0x2000;
        *(int *)(state2 + 0x4C) = flags;
        *(int *)(actor2 + 0x98) &= ~0x100;
        Battle_FlushScriptSounds(actor2);
    }
}
