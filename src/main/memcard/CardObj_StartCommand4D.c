/* MASPSX_FLAGS: --stack-return-delay */

extern int (*g_MemCardObjLookupFn)(void);

void CardObj_BeginCommand4D(int arg0, int arg1);

void CardObj_StartCommand4D(int arg0, int arg1) {
    CardObj_BeginCommand4D(g_MemCardObjLookupFn(), arg1);
}
