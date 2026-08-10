int func_8018F284(char *obj, int query, unsigned int op,
                  int a, int b, int c) {
    char *state = obj + 0xC;
    FieldActorNode *node;

    switch (op) {
    case 19:
        if (query == 0) {
            RW8(obj, 0x3) = a;
        } else {
            *(int *)a = RW8(obj, 0x3);
            *(int *)b = RW16(state, 0x9A);
        }
        break;
    case 25:
        if (query == 1) {
            RW32(state, 0x4) = a;
            *(int *)a = query;
        }
        break;
    case 0:
        RW32(state, 0x80) = (int)D_8009D20C;
        while (RW32(state, 0x80) != 0) {
            node = (FieldActorNode *)RW32(state, 0x80);
            if (node->b0C == a && node->b0D == b && !(node->w98 & 0x10)) {
                break;
            }
            RW32(state, 0x80) =
                (int)((FieldActorNode *)RW32(state, 0x80))->next;
        }
        break;
    case 17:
        RW32(state, 0x40) = a;
        RW32(state, 0x48) = c;
        if (b == -1) {
            RW32(state, 0x44) = RW32(D_8009D254, 0x2C);
        } else {
            RW32(state, 0x44) = b;
        }
        RW32(state, 0x80) = 0;
        break;
    case 26:
        RW32(state, 0x40) = a;
        RW32(state, 0x44) = RW32((void *)RW32(obj, 0x8), 0x2C);
        RW32(state, 0x48) = b;
        RW32(state, 0x4C) = c;
        RW32(state, 0x80) = 0;
        break;
    case 4:
        RW32(state, 0x88) = a;
        break;
    case 2:
        RW32(state, 0x8C) = a;
        break;
    case 3:
        RW32(state, 0x90) = a;
        RW16(state, 0xA6) = b;
        if (c > 0) {
            RW16(state, 0xA4) = c;
        }
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
        RW8(state, 0xA8) = c;
        RW32(state, 0x0) = (int)RoomLib_NotifyArmB_8018F598;
        break;
    case 11:
        RW8(state, 0xB) = a;
        RW8(state, 0xC) = b;
        break;
    case 6:
        RW16(state, 0x70) = a >> 16;
        RW16(state, 0x72) = b >> 16;
        RW16(state, 0x74) = c >> 16;
        break;
    case 20:
        RW16(state, 0x9C) = a;
        RW16(state, 0x9E) = b;
        RW16(state, 0xA0) = c;
        break;
    case 21:
        RW32(state, 0x84) = (int)D_8009D20C;
        while (RW32(state, 0x84) != 0) {
            node = (FieldActorNode *)RW32(state, 0x84);
            if (node->b0C == a && node->b0D == b && !(node->w98 & 0x10)) {
                break;
            }
            RW32(state, 0x84) =
                (int)((FieldActorNode *)RW32(state, 0x84))->next;
        }
        RW16(state, 0xA2) = c & 0xFFF;
        RW8(state, 0xA9) = (unsigned int)c >> 31;
        break;
    case 22:
        RW16(state, 0xA2) = c & 0xFFF;
        RW32(state, 0x60) = a;
        RW32(state, 0x68) = b;
        RW8(state, 0xA9) = (unsigned int)c >> 31;
        break;
    case 30:
        RW8(state, 0xAA) = a;
        break;
    case 31:
        RW8(state, 0xAB) = a;
        RW8(state, 0xAC) = b;
        break;
    }
    return 0;
}
