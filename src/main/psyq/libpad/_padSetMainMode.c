/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/card_obj.h"

extern int (*g_MemCardIsTransferActiveFn)(CardObj *obj);

void CardObj_EmitReadIdCommand(CardObj *obj);
int CardObj_CheckAbortOrDispatch(CardObj *obj);

int CardObj_BeginReadIdCommand(CardObj *obj, int byte1, int byte2) {
    register int compareByte asm("$19");
    register int flag asm("$3");
    register int result asm("$2");

    compareByte = byte1;
    result = g_MemCardIsTransferActiveFn(obj);
    if (result != 0) {
        return 0;
    }

    result = 1;
    asm("" : "+r"(result));
    flag = 1;
    obj->field_46 = flag;
    obj->fn_14 = (void (*)(void *))CardObj_EmitReadIdCommand;
    obj->fn_18 = CardObj_CheckAbortOrDispatch;
    obj->field_51 = byte1;
    obj->field_52 = byte2;
    flag = (compareByte & 0xFF) ^ obj->field_e4;
    flag = (unsigned int)flag < 1;
    obj->field_53 = flag;
    return result;
}
