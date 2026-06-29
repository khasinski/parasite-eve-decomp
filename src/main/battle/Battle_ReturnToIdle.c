extern int g_FieldMoveLock;
extern char *g_ActiveActor;
extern char *g_PlayerEntity;

void Entity_SetActionMode(char *arg0, int arg1);

void Battle_ReturnToIdle(void) {
    char *state;
    char *actor;
    int mask_state;
    int flags;

    mask_state = 0xFFFEFFFF;
    g_FieldMoveLock &= -2;
    state = g_ActiveActor;
    actor = g_PlayerEntity;
    flags = *(int *)(state + 0x4C);
    flags &= mask_state;
    *(int *)(state + 0x4C) = flags;
    *(int *)(actor + 0x68) = 0;
    *(int *)(actor + 0x6C) = 0;
    *(int *)(actor + 0x70) = 0;
    Entity_SetActionMode(actor, *(unsigned char *)(state + 0x12));
}
