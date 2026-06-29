/* MASPSX_FLAGS: --stack-return-delay */
extern int (*g_MemCardStateDispatchFn)(int);
extern int g_MemCardDispatchResult;

void MemCard_ReadByte(int arg0, int arg1);

void MemCard_ReadByteWithCallbackValue(int arg0) {
    g_MemCardDispatchResult = g_MemCardStateDispatchFn(arg0);
    MemCard_ReadByte(arg0, -2);
}
