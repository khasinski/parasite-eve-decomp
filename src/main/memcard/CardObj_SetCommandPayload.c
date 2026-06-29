/* MASPSX_FLAGS: --stack-return-delay */

extern int (*g_MemCardObjLookupFn)(void);

void CardObj_SetPayload4D(int arg0, int arg1, int arg2);

void CardObj_SetCommandPayload(int arg0, int arg1, int arg2) {
    CardObj_SetPayload4D(g_MemCardObjLookupFn(), arg1, arg2);
}
