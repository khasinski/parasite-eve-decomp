extern int *g_CurrentEntity;

void Entity_ResolvePosition(int *arg0, unsigned int arg1);

int Entity_ResolveCurrentPosition(unsigned short **arg0) {
    Entity_ResolvePosition(g_CurrentEntity, *arg0[0]);
    return 1;
}
