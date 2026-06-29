extern char *g_CurrentEntity;

int Task_ClearEntityShadowFlag(void) {
    *(int *)(g_CurrentEntity + 0x98) &= 0xDFFFFFFF;
    return 1;
}
