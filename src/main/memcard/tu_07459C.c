extern int (*g_MemCardObjResetFn)(void *);

void CardObj_EmitReadIdCommand(void *arg0)
{
    int state = *((unsigned char *)arg0 + 0x46);

    switch (state) {
    case 2:
        *((unsigned char *)arg0 + 0x36) = 0x44;
        *(void **)((char *)arg0 + 0x2C) = (char *)arg0 + 0x51;
        *((unsigned char *)arg0 + 0x35) = state;
        break;
    case 3:
        *((unsigned char *)arg0 + 0x36) = 0x4D;
        *(void **)((char *)arg0 + 0x2C) = (char *)arg0 + 0x5D;
        *((unsigned char *)arg0 + 0x35) = 6;
        break;
    }
}

int CardObj_CheckAbortOrDispatch(void *arg0)
{
    if (*((unsigned char *)arg0 + 0x53) != 0) {
        if (*((unsigned char *)arg0 + 0x46) == 2) {
            return 1;
        }

        *((unsigned char *)arg0 + 0x46) = 0xFE;
        return 0;
    }

    g_MemCardObjResetFn(arg0);
    return 0;
}

void CardObj_EmitCommand43(unsigned char *arg0, unsigned char arg1) {
    arg0[0x36] = 0x43;
    *(unsigned char **)(arg0 + 0x2C) = arg0 + 0x24;
    arg0[0x24] = arg1;
    arg0[0x35] = 1;
}

void CardObj_EmitCommand45(unsigned char *arg0) {
    arg0[0x36] = 0x45;
    *(int *)(arg0 + 0x2C) = 0;
    arg0[0x35] = 0;
}

void CardObj_EmitCommand4C(unsigned char *arg0, unsigned char arg1) {
    arg0[0x36] = 0x4C;
    *(unsigned char **)(arg0 + 0x2C) = arg0 + 0x24;
    arg0[0x24] = arg1;
    arg0[0x35] = 1;
}

void CardObj_EmitCommand46(unsigned char *arg0, unsigned char arg1) {
    arg0[0x36] = 0x46;
    *(unsigned char **)(arg0 + 0x2C) = arg0 + 0x24;
    arg0[0x24] = arg1;
    arg0[0x35] = 1;
}

void CardObj_EmitCommand47(unsigned char *arg0, unsigned char arg1) {
    arg0[0x36] = 0x47;
    *(unsigned char **)(arg0 + 0x2C) = arg0 + 0x24;
    arg0[0x24] = arg1;
    arg0[0x35] = 1;
}

void CardObj_EmitCommand4B(unsigned char *arg0) {
    arg0[0x36] = 0x4B;
    *(int *)(arg0 + 0x2C) = 0;
    arg0[0x35] = 0;
}
