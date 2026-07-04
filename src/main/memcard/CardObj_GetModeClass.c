extern int (*g_MemCardObjLookupFn)(void);

/* The duplicated `return 1` bodies for cases 3 and 2 are load-bearing: a
 * shared case body makes GCC merge {2,3} into one range compare, while
 * retail's case tree tests ==3, <4, ==2, ==6 separately. */

int CardObj_GetModeClass(void) {
    int obj;

    obj = g_MemCardObjLookupFn();
    if ((*(unsigned int *)(obj + 0x34) & 0xFFFF0000) == 0
        && (obj == *(int *)(obj + 0x10) || *(unsigned char *)(obj + 0x38) == 0)
        && **(unsigned char **)(obj + 0x30) == 0) {
        return *(unsigned char *)(obj + 0x49);
    }

    switch (*(unsigned char *)(obj + 0x49)) {
    case 3:
        return 1;
    case 2:
        return 1;
    case 6:
        return 4;
    default:
        return *(unsigned char *)(obj + 0x49);
    }
}
