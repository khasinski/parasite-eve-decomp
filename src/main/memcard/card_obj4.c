/* GCC_VERSION: 2.8.1 */

#include "pe1/card_obj.h"

extern int (*D_8009B740)(CardObj *obj);

void func_80083C20(void *obj);
void func_80083C3C(void *obj);

int card_obj4(CardObj *obj, int command) {
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
