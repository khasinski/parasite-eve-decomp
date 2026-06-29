extern int (*g_MemCardObjResetFn)(void *);

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
