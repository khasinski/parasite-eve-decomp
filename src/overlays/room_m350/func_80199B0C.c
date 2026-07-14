typedef int s32;

extern char *D_8009D20C;
extern char *D_8019A774;

char **func_80199B0C(s32 arg0, s32 arg1, s32 arg2) {
    char **slot;
    char **iter;
    char *node;

    slot = &D_8019A774;
    if ((*slot = D_8009D20C) != 0) {
        iter = slot;
        while (1) {
            node = D_8019A774;
            if ((node[0xC] == arg1) && (node[0xD] == arg2) && ((*(s32 *)(node + 0x98) & 0x10) == 0)) {
                break;
            }

            if ((*iter = *(char **)(D_8019A774 + 4)) == 0) {
                break;
            }
        }
    }

    return &D_8019A774;
}
