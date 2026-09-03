#include "pe1/card_obj.h"

void CardObj_EmitCommand43(CardObj *obj, unsigned char value);
void CardObj_EmitReadCommandForState(CardObj *obj);

int func_80084B78(CardObj *obj) {
    register int compare asm("$2");
    int state;

    compare = 0xF3;
    if (obj->response_3c[0] == compare) {
        compare = obj->field_e8;
        if (compare == 0) {
            goto emit_zero;
        }
    }

    state = obj->field_46;
    compare = 1;
    if (state == compare) {
        goto emit_one;
    }
    compare = state < 2;
    if (compare == 0) {
        goto high_state;
    }
    if (state == 0) {
        goto done;
    }
    goto dispatch;

high_state:
    compare = 0xFE;
    if (state == compare) {
        goto emit_zero;
    }
    compare = 0xFF;
    if (state == compare) {
        goto done;
    }
    goto dispatch;

emit_one:
    asm volatile("" : : "r"(compare));
    CardObj_EmitCommand43(obj, 1);
    goto done;

emit_zero:
    asm volatile("" : : "r"(compare));
    CardObj_EmitCommand43(obj, 0);
    goto done;

dispatch:
    if (obj->fn_14 != 0) {
        obj->fn_14(obj);
    } else {
        CardObj_EmitReadCommandForState(obj);
    }

done:
    return 0;
}
