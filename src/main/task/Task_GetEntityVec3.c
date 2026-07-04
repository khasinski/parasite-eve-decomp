/* CC1_FLAGS: -G8 -g3 */
extern int *g_CurrentEntity;

int Task_GetEntityVec3(int **args) {
    switch (*args[0]) {
    case 0:
        *args[1] = g_CurrentEntity[0x28 / 4];
        *args[2] = g_CurrentEntity[0x2C / 4];
        *args[3] = g_CurrentEntity[0x30 / 4];
        break;
    case 1:
        *args[1] = g_CurrentEntity[0x40 / 4];
        *args[2] = g_CurrentEntity[0x44 / 4];
        *args[3] = g_CurrentEntity[0x48 / 4];
        break;
    case 2:
        *args[1] = g_CurrentEntity[0x68 / 4];
        *args[2] = g_CurrentEntity[0x6C / 4];
        *args[3] = g_CurrentEntity[0x70 / 4];
        break;
    case 3:
        *args[1] = g_CurrentEntity[0x78 / 4];
        *args[2] = g_CurrentEntity[0x7C / 4];
        *args[3] = g_CurrentEntity[0x80 / 4];
        break;
    case 4:
        *args[1] = g_CurrentEntity[0x88 / 4];
        *args[2] = g_CurrentEntity[0x8C / 4];
        *args[3] = g_CurrentEntity[0x90 / 4];
        break;
    case 5:
        *args[1] = ((short *)g_CurrentEntity)[0x38 / 2];
        *args[2] = ((short *)g_CurrentEntity)[0x3A / 2];
        *args[3] = ((short *)g_CurrentEntity)[0x3C / 2];
        break;
    case 6:
        *args[1] = g_CurrentEntity[0x58 / 4];
        *args[2] = g_CurrentEntity[0x5C / 4];
        *args[3] = g_CurrentEntity[0x60 / 4];
        break;
    }
    return 1;
}
