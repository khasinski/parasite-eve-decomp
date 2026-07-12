typedef unsigned char u8;

int FieldEng_GetStatus(char *obj);

extern char *D_8009D254;

int func_800C6C18(char *obj) {
    int enabled;

    if (FieldEng_GetStatus(obj) == 3) {
        enabled = (*(u8 *)(*(char **)(*(char **)(obj + 8) + 0) + 0x18) == 2);
    } else {
        enabled = 1;
    }

    if (enabled != 0) {
        if (FieldEng_GetStatus(obj) == 3) {
            *(int *)(*(char **)D_8009D254 + 0x4C) |= 0x4000;
            *(int *)(*(char **)(*(char **)(obj + 8) + 0) + 0) |= 0x80000000;
        }
    }

    return enabled;
}
