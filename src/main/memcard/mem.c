/* MASPSX_FLAGS: --stack-return-delay */
#include "include_asm.h"

#include "pe1/card_obj.h"

typedef unsigned char u8;

void CardObj_EmitCommand43(CardObj *arg0, unsigned char arg1);
void CardObj_EmitReadCommandForState(void);
int CardObj_IsTransferActive(CardObj *obj);
int CardObj_AdvanceReadLayout(void *obj);

extern void (*g_MemCardObjResetFn)(void *);

int MemCard_StateDispatch(CardObj *obj) {
    void (*callback)(void *);
    int state;

    if (obj->response_3c[0] == 0xF3) {
        if (obj->field_e8 == 0) {
            goto call_zero;
        }
    }

    state = obj->field_46;
    if (state == 1) {
        goto call_one;
    }
    asm volatile("");
    if (state < 2) {
        if (state == 0) {
            goto done;
        }
    } else {
        if (state == 0xFE) {
            goto call_zero;
        }
        asm volatile("");
        if (state == 0xFF) {
            goto done;
        }
    }
    goto dispatch_callback;

call_one:
    CardObj_EmitCommand43(obj, 1);
    goto done;

call_zero:
    CardObj_EmitCommand43(obj, 0);
    goto done;

dispatch_callback:
    callback = obj->fn_14;
    if (callback != 0) {
        callback(obj);
        goto done;
    } else {
        CardObj_EmitReadCommandForState();
    }

done:
    return 0;
}

void CardObj_HandleResponse(CardObj *obj) {
    int header;
    int saved_e8;
    int count;
    int state;
    int delta;
    register int one asm("$4");
    int (*callback)(void *);

    header = obj->response_3c[0] >> 4;
    saved_e8 = obj->field_e8;
    obj->field_e8 = header;
    if (header == 0xF) {
        obj->field_e8 = saved_e8;
    } else {
        obj->output_30[0] = 0;
        obj->output_30[1] = obj->response_3c[0];
        count = 2;
        if (count < obj->pad_40[4]) {
            do {
                obj->output_30[count] = obj->response_3c[count];
                count++;
            } while (count < obj->pad_40[4]);
        }
    }

    if (obj->response_3c[1] != 0 || (obj->field_46 == 1 && obj->fn_14 == 0) || obj->pad_50[0] != 0) {
        if (CardObj_IsTransferActive(obj) == 0 && obj->saved_command == 0 && obj->field_4a == 0 && obj->field_e8 != saved_e8) {
            g_MemCardObjResetFn(obj);
        }
    } else {
        g_MemCardObjResetFn(obj);
    }

    obj->field_4a = 0;
    if ((unsigned char)(obj->field_46 - 2) < 0xFC && obj->response_3c[0] != 0xF3) {
        g_MemCardObjResetFn(obj);
    }

    state = obj->field_46;
    if (state == 0) {
        goto dispatch_state;
    }
    if (state == 0xFF) {
        goto dispatch_state;
    }
    if (obj->command == 0) {
        return;
    }

dispatch_state:
    one = 1;
    if (state == one) {
        goto state_one;
    }
    if (state < 2) {
        if (state == 0) {
            goto state_zero;
        }
        goto state_default;
    }
    if (state == 0xFE) {
        goto state_fe;
    }
    if (state == 0xFF) {
        return;
    }
    goto state_default;

state_zero:
    if (obj->field_e8 == 0) {
        return;
    }
    obj->field_49 = one;
    obj->field_46++;
    return;

state_one:
    obj->field_47 = 0;
    obj->field_46++;
    return;

state_fe:
    obj->field_46 = 0xFF;
    return;

state_default:
    callback = (int (*)(void *))obj->fn_18;
    if (callback != 0) {
        delta = callback(obj);
    } else {
        delta = CardObj_AdvanceReadLayout(obj);
    }
    obj->field_46 += delta;
}
void MemCard_OutputHandler(CardObj *obj) {
    int state;
    int counter;

    obj->field_4c = obj->field_4c + 1;
    state = obj->field_46;

    if (state == 0) {
        goto maybe_reset_output;
    }
    if (state == 1) {
        counter = obj->field_4a;
        if (counter < 2U) {
            goto bump_counter;
        }
        obj->field_49 = 2;
        obj->field_46 = 0xFF;
        goto done;
    }

    counter = obj->field_4a;
    if (counter >= 4U) {
        goto maybe_call_callback;
    }
    goto bump_counter;

bump_counter:
    obj->field_4a = counter + 1;
    goto done;

maybe_call_callback:
    if (obj->field_49 != 0) {
        g_MemCardObjResetFn(obj);
    }

maybe_reset_output:
    if (obj->response_3c[0] != 0xF3) {
        int reset;
        register u8 *out_reg asm("$2");

        reset = 0xFF;
        out_reg = obj->output_30;
        out_reg[0] = reset;
        out_reg = obj->output_30;
        out_reg[1] = 0;
        obj->field_e8 = 0;
    }

done:
    return;
}

int CardObj_IsTransferActive(CardObj *obj) {
    int value;
    int field;

    if (obj->field_e6 != 0) {
        value = 0xFF;
        field = obj->field_46;
        if (field == value) {
            goto ret_zero;
        }
    }
    return 1;

ret_zero:
    return 0;
}
