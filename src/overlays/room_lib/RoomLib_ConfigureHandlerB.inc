#ifndef ROOMLIB_CONFIGURE_HANDLER_B_NAME
#define ROOMLIB_CONFIGURE_HANDLER_B_NAME func_80190A94
#endif
#ifndef ROOMLIB_CONFIGURE_HANDLER_B_ARM
#define ROOMLIB_CONFIGURE_HANDLER_B_ARM RoomLib_ArmWindowA_80190D0C
#endif
#ifndef ROOMLIB_CONFIGURE_HANDLER_B_ADVANCE
#define ROOMLIB_CONFIGURE_HANDLER_B_ADVANCE RoomLib_AdvanceArcToTarget_80191110
#endif

int ROOMLIB_CONFIGURE_HANDLER_B_NAME(char *obj, int query, unsigned int op,
                  int a, int b, int c) {
    char *state = obj + 0xC;
    FieldActorNode *node;

    switch (op) {
    case 19:
        if (query == 0) {
            RW8(obj, 0x3) = a;
        } else {
            *(int *)a = RW8(obj, 0x3);
        }
        break;
    case 25:
        if (query == 1) {
            RW32(state, 0x4) = a;
            *(int *)a = query;
        }
        break;
    case 4:
        RW32(state, 0x68) = a;
        RW32(state, 0x6C) = b;
        break;
    case 2:
        RW32(state, 0x70) = a;
        break;
    case 12:
        RW32(state, 0x74) = a;
        break;
    case 13:
        RW32(state, 0x80) = a;
        break;
    case 14:
        RW16(state, 0x86) = b;
        break;
    case 6:
        RW32(state, 0x40) = a;
        RW32(state, 0x44) = b;
        RW32(state, 0x48) = c;
        break;
    case 23:
        a = a != 0;
        b = (b != 0) << 1;
        c = (c != 0) << 2;
        RW8(state, 0xD) = a | b | c;
        break;
    case 10:
        RW8(state, 0xA) = a;
        RW16(state, 0x8) = b;
        RW32(state, 0x0) = (int)ROOMLIB_CONFIGURE_HANDLER_B_ARM;
        break;
    case 11:
        RW8(state, 0xB) = a;
        RW8(state, 0xC) = b;
        break;
    case 16:
        RW16(state, 0x88) = a;
        break;
    case 18:
        if (RW8(obj, 0x3) == 2) {
            RW32(state, 0x0) = (int)ROOMLIB_CONFIGURE_HANDLER_B_ADVANCE;
            RW32(state, 0x74) = a;
            RW8(obj, 0x3) = 3;
        }
        break;
    case 0:
        RW32(state, 0x60) = (int)D_8009D20C;
        while (RW32(state, 0x60) != 0) {
            node = (FieldActorNode *)RW32(state, 0x60);
            if (node->b0C == a && node->b0D == b && !(node->w98 & 0x10)) {
                break;
            }
            RW32(state, 0x60) =
                (int)((FieldActorNode *)RW32(state, 0x60))->next;
        }
        break;
    case 17:
        RW32(state, 0x30) = a;
        RW32(state, 0x38) = c;
        if (b != -1) {
            RW32(state, 0x34) = b;
        } else {
            RW32(state, 0x34) = RW32(D_8009D254, 0x2C);
        }
        RW32(state, 0x60) = 0;
        break;
    case 21:
        RW32(state, 0x64) = (int)D_8009D20C;
        while (RW32(state, 0x64) != 0) {
            node = (FieldActorNode *)RW32(state, 0x64);
            if (node->b0C == a && node->b0D == b && !(node->w98 & 0x10)) {
                break;
            }
            RW32(state, 0x64) =
                (int)((FieldActorNode *)RW32(state, 0x64))->next;
        }
        RW16(state, 0x8A) = c;
        break;
    case 22:
        RW32(state, 0x50) = a;
        RW32(state, 0x58) = b;
        RW16(state, 0x8A) = c;
        break;
    }
    return 0;
}

#undef ROOMLIB_CONFIGURE_HANDLER_B_ADVANCE
#undef ROOMLIB_CONFIGURE_HANDLER_B_ARM
#undef ROOMLIB_CONFIGURE_HANDLER_B_NAME
