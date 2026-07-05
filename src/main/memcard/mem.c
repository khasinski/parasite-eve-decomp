/* MASPSX_FLAGS: --stack-return-delay */
#include "include_asm.h"

typedef unsigned char u8;

void CardObj_EmitCommand43(void *arg0, int arg1);
void CardObj_EmitReadCommandForState(void);

extern void (*g_MemCardObjResetFn)(void *);

#include "pe1/card_obj.h"

int MemCard_StateDispatch(u8 *obj) {
    void (*callback)(void *);
    int state;

    if (((u8 *)*(void **)(obj + 0x3C))[0] == 0xF3) {
        if (obj[0xE8] == 0) {
            goto call_zero;
        }
    }

    state = obj[0x46];
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
    callback = *(void **)(obj + 0x14);
    if (callback != 0) {
        callback(obj);
        goto done;
    } else {
        CardObj_EmitReadCommandForState();
    }

done:
    return 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem", CardObj_HandleResponse);

void MemCard_OutputHandler(u8 *obj) {
    int state;
    int counter;

    *(int *)(obj + 0x4C) = *(int *)(obj + 0x4C) + 1;
    state = obj[0x46];

    if (state == 0) {
        goto maybe_reset_output;
    }
    if (state == 1) {
        counter = obj[0x4A];
        if (counter < 2U) {
            goto bump_counter;
        }
        obj[0x49] = 2;
        obj[0x46] = 0xFF;
        goto done;
    }

    counter = obj[0x4A];
    if (counter >= 4U) {
        goto maybe_call_callback;
    }
    goto bump_counter;

bump_counter:
    obj[0x4A] = counter + 1;
    goto done;

maybe_call_callback:
    if (obj[0x49] != 0) {
        g_MemCardObjResetFn(obj);
    }

maybe_reset_output:
    if (((u8 *)*(void **)(obj + 0x3C))[0] != 0xF3) {
        int reset;
        register u8 *out_reg asm("$2");

        reset = 0xFF;
        out_reg = *(void **)(obj + 0x30);
        out_reg[0] = reset;
        out_reg = *(void **)(obj + 0x30);
        out_reg[1] = 0;
        obj[0xE8] = 0;
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
