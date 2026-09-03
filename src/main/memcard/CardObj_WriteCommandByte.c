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
