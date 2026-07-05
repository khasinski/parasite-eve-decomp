extern char *g_CurrentEntity;

int Task_SetEntityShadowFlag(void) {
    *(int *)(g_CurrentEntity + 0x98) |= 0x20000000;
    return 1;
}

int Task_ClearEntityShadowFlag(void) {
    *(int *)(g_CurrentEntity + 0x98) &= 0xDFFFFFFF;
    return 1;
}
