int func_8018FFD0(char *obj, int query, unsigned int op,
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
        RW32(state, 0x20) = a;
        RW32(state, 0x28) = c;
        if (b == -1) {
            RW32(state, 0x24) = RW32(D_8009D254, 0x2C);
        } else {
            RW32(state, 0x24) = b;
        }
        RW32(state, 0x60) = 0;
        break;
    case 4:
        RW32(state, 0x68) = a;
        RW16(state, 0x70) = b;
        RW16(state, 0x6C) = 0;
        break;
    case 6:
        RW32(state, 0x40) = a;
        RW32(state, 0x44) = b;
        RW32(state, 0x48) = c;
        break;
    case 15:
        RW16(state, 0x6C) = a;
        RW16(state, 0x70) = b;
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
        RW16(state, 0x6E) = c;
        break;
    case 22:
        RW32(state, 0x50) = a;
        RW32(state, 0x58) = b;
        RW16(state, 0x6E) = c;
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
        RW16(state, 0x74) = c;
        RW32(state, 0x0) = (int)RoomLib_Notify2ArmB_801902D8;
        break;
    case 11:
        RW8(state, 0xB) = a;
        RW8(state, 0xC) = b;
        break;
    case 24:
        if (query == 1) {
            *(int *)a = RW16(state, 0x72);
        }
        break;
    case 27:
        RW16(state, 0x7E) = a;
        RW16(state, 0x80) = b;
        RW16(state, 0x82) = c;
        if (a != -1) {
            if (b != -1) {
                if (c == -1) {
                    RW16(state, 0x76) = 2;
                } else {
                    RW16(state, 0x76) = 3;
                }
            } else {
                RW16(state, 0x76) = 1;
            }
        } else {
            RW16(state, 0x76) = 0;
        }
        break;
    case 31:
        RW8(state, 0x84) = a;
        break;
    case 32:
        RW8(state, 0x85) = a;
        break;
    }
    return 0;
}
