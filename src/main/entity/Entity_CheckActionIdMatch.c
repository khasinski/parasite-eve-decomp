typedef signed char s8;
typedef unsigned short u16;
typedef unsigned int u32;

void Entity_CheckActionIdMatch(char **arg0, void *arg1, char **arg2, int arg3) {
    char *entry;
    int i;
    int kind;
    int offset;
    int value;

    entry = *arg2;
    if (entry == 0) {
        return;
    }

    if (**(char **)(entry + 0x18) != 1) {
        return;
    }

    kind = (*(u32 *)entry >> 21) & 7;
    if (kind >= 3) {
        return;
    }

    if (*(int *)(entry + 0x10) <= 0) {
        return;
    }

    i = 0;
    arg3 = (short)arg3;
    do {
        offset = (*(u32 *)entry >> 19) & 0x1C;
        value = *(s8 *)(offset + (int)entry + (i & 0xFFFF) + 0x7C);
        if (arg3 == value) {
            *(int *)(*arg0 + 0x4C) |= 0x4000;
            *(u32 *)entry |= 0x80000000;
            return;
        }

        if (value < 0) {
            return;
        }

        i++;
    } while ((u16)i < 4);
}
