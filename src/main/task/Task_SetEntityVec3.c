/* CC1_FLAGS: -g3 */
extern int *g_CurrentEntity;
extern int *g_PlayerEntity;
extern int g_RenderStateFlags[];

void Entity_FindFloor(void);

int Task_SetEntityVec3(int **args) {
    int **argp = args;

    switch (*argp[0]) {
    case 0:
        {
            register int *state asm("$4") = g_CurrentEntity;
            state[0x28 / 4] = *argp[1];
            state[0x2C / 4] = *argp[2];
            state[0x30 / 4] = *argp[3];
        }
        Entity_FindFloor();
        g_CurrentEntity[0x40 / 4] = g_CurrentEntity[0x28 / 4];
        g_CurrentEntity[0x44 / 4] = g_CurrentEntity[0x2C / 4];
        g_CurrentEntity[0x48 / 4] = g_CurrentEntity[0x30 / 4];
        if (g_CurrentEntity == g_PlayerEntity) {
            g_RenderStateFlags[0] |= 0x80;
        }
        break;
    case 1:
        g_CurrentEntity[0x40 / 4] = *argp[1];
        g_CurrentEntity[0x44 / 4] = *argp[2];
        g_CurrentEntity[0x48 / 4] = *argp[3];
        break;
    case 2:
        g_CurrentEntity[0x68 / 4] = *argp[1];
        g_CurrentEntity[0x6C / 4] = *argp[2];
        g_CurrentEntity[0x70 / 4] = *argp[3];
        break;
    case 3:
        g_CurrentEntity[0x78 / 4] = *argp[1];
        g_CurrentEntity[0x7C / 4] = *argp[2];
        g_CurrentEntity[0x80 / 4] = *argp[3];
        break;
    case 4:
        g_CurrentEntity[0x88 / 4] = *argp[1];
        g_CurrentEntity[0x8C / 4] = *argp[2];
        g_CurrentEntity[0x90 / 4] = *argp[3];
        break;
    case 5:
        ((short *)g_CurrentEntity)[0x38 / 2] = *argp[1];
        ((short *)g_CurrentEntity)[0x3A / 2] = *argp[2];
        ((short *)g_CurrentEntity)[0x3C / 2] = *argp[3];
        break;
    case 6:
        g_CurrentEntity[0x58 / 4] = *argp[1];
        g_CurrentEntity[0x5C / 4] = *argp[2];
        g_CurrentEntity[0x60 / 4] = *argp[3];
        break;
    }
    return 1;
}
