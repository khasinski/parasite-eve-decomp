/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;
typedef int s32;

typedef s32 (*Callback)(s32);

s32 MemCard_WriteByte(void *arg0, s32 arg1);
extern Callback g_MemCardStateDispatchFn;
extern s32 g_MemCardCommandByte;
extern s32 g_MemCardWriteBlockCount;
extern s32 g_MemCardDispatchResult;

s32 MemCard_WriteDataAndGetSize(void *arg0) {
    s32 status;
    s32 arg1;
    s32 low;

    if (g_MemCardDispatchResult != 0) {
        g_MemCardStateDispatchFn(*(s32 *)(arg0 + 0xC) + 0x1E0);
        g_MemCardStateDispatchFn(*(s32 *)(arg0 + 0xC) + 0x2D0);
    }

    arg1 = 0;
    if (*(u8 *)(arg0 + 0x36) == 0) {
        arg1 = g_MemCardCommandByte;
    }

    status = MemCard_WriteByte(arg0, arg1);
    if (status < 0) {
        return status;
    }

    if ((status & 0xF0) == 0) {
        return -9;
    }

    low = (status & 0xF) * 2;
    g_MemCardWriteBlockCount = low;
    if (low != 0) {
        return 0;
    }

    g_MemCardWriteBlockCount = 0x20;
    return 0;
}
