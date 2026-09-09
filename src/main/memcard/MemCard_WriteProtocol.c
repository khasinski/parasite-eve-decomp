/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/card_obj.h"

extern void (*D_8009B750)(void);
extern void (*D_8009B754)(void);
extern int D_8009B760;
extern int D_8009B764;
extern int g_MemCardPort1Present;
extern void (*g_MemCardStateDispatchFn)(void *);
extern int g_MemCardDispatchResult;
extern int g_MemCardCommandByte;
extern int g_MemCardWriteBlockCount;

int MemCard_WriteByte(CardObj *obj, int value);

void CardObj_WriteCommandByte(CardObj *obj) {
    if (D_8009B764 == g_MemCardPort1Present && D_8009B760 != 0) {
        D_8009B754();
        D_8009B750();
    }

    if (g_MemCardDispatchResult != 0) {
        g_MemCardStateDispatchFn(obj->field_0c);
        g_MemCardStateDispatchFn(obj->field_0c + 0xF0);
    }

    if (obj->command == 0) {
        MemCard_WriteByte(obj, 0x42);
    } else {
        MemCard_WriteByte(obj, obj->command);
    }
}

int MemCard_WriteDataAndGetSize(CardObj *obj) {
    int result;
    int blockCount;
    int value;

    if (g_MemCardDispatchResult != 0) {
        g_MemCardStateDispatchFn(obj->field_0c + 0x1E0);
        g_MemCardStateDispatchFn(obj->field_0c + 0x2D0);
    }

    value = 0;
    if (obj->command == 0) {
        value = g_MemCardCommandByte;
    }

    result = MemCard_WriteByte(obj, value);
    if (result < 0) {
        return result;
    }
    if ((result & 0xF0) == 0) {
        return -9;
    }

    blockCount = (result & 0xF) << 1;
    g_MemCardWriteBlockCount = blockCount;
    if (blockCount == 0) {
        g_MemCardWriteBlockCount = 0x20;
    }
    return 0;
}
