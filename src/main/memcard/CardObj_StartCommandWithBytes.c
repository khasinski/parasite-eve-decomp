/* MASPSX_FLAGS: --stack-return-delay */

extern int (*g_MemCardObjLookupFn)(void);

void CardObj_BeginReadIdCommand(int arg0, int arg1, int arg2);

void CardObj_StartCommandWithBytes(int arg0, unsigned char arg1, unsigned char arg2) {
    CardObj_BeginReadIdCommand(g_MemCardObjLookupFn(), arg1, arg2);
}
