extern char *g_CurrentEntity;

void Entity_AllocSlot(void *arg0);

int Task_InitEntityMoveState(void) {
    Entity_AllocSlot(g_CurrentEntity);
    return 1;
}
