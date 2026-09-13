/* GCC_VERSION: 2.8.1 */

#include "pe1/card_obj.h"

extern int (*D_8009B740)(CardObj *obj);

void func_80083C20(void *obj);
int func_80083C3C(CardObj *obj);

int CardObj_BeginCommand(CardObj *obj, int command) {
    register int result asm("$2");
    register int active asm("$3");

    result = D_8009B740(obj);
    if (result != 0) {
        result = 0;
    } else {
        result = 1;
        asm volatile("" : "+r"(result));
        active = 1;
        obj->field_46 = active;
        obj->fn_14 = func_80083C20;
        obj->field_20 = command;
        obj->fn_18 = func_80083C3C;
    }
    return result;
}

void CardObj_EmitCommand4D(CardObj *arg0) {
    int value = arg0->field_20;

    arg0->command = 0x4D;
    arg0->payload_2c_len = 6;
    arg0->payload_2c = (unsigned char *)value;
}
