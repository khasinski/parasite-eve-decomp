extern char *g_PlayerEntity;

int ROOMLIB_CONFIGURE_WINDOW_NAME(char *obj, int query, unsigned int op,
                                  int a, int b, int c) {
    char *state = obj + 0xC;
    FieldActorNode *node;
    register int callback asm("$3");
    int sentinel;

    switch (op) {
    case 19:
        if (query == 0) {
            RW8(obj, 0x3) = a;
        } else {
            *(int *)a = RW8(obj, 0x3);
        }
        break;
    case 0:
        RW32(state, 0x70) = (int)g_FieldActorListHead;
        while (RW32(state, 0x70) != 0) {
            node = (FieldActorNode *)RW32(state, 0x70);
            if (node->b0C == a && node->b0D == b && !(node->w98 & 0x10)) {
                break;
            }
            RW32(state, 0x70) =
                (int)((FieldActorNode *)RW32(state, 0x70))->next;
        }
        break;
    case 17:
        RW32(state, 0x40) = a;
        RW32(state, 0x48) = c;
        if (b == -1) {
            RW32(state, 0x44) = RW32(g_PlayerEntity, 0x2C);
        } else {
            RW32(state, 0x44) = b;
        }
        RW32(state, 0x70) = 0;
        break;
    case 4:
        RW32(state, 0x74) = a;
        break;
    case 2:
        RW32(state, 0x78) = a;
        break;
    case 3:
        RW32(state, 0x7C) = a;
        break;
    case 23:
        a = a != 0;
        b = (b != 0) << 1;
        c = (c != 0) << 2;
        RW8(state, 0xD) = a | b | c;
        break;
    case 10:
        callback = (int)ROOMLIB_CONFIGURE_WINDOW_CALLBACK;
        sentinel = -1;
        RW8(state, 0xA) = a;
        RW16(state, 0x8) = b;
        PE1_COMPILER_MEMORY_BARRIER();
        RW32(state, 0x0) = callback;
        if (a != sentinel) {
            PE1_COMPILER_MEMORY_BARRIER();
            break;
        }
        return 0;
    case 11:
        RW8(state, 0xB) = a;
        break;
    case 6:
        RW16(state, 0x60) = a >> 16;
        RW16(state, 0x62) = b >> 16;
        RW16(state, 0x64) = c >> 16;
        break;
    }
    return 0;
}
