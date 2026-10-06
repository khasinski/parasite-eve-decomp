/* ASSEMBLER: GNU */
/* Psy-Q LIBPAD PADSEQD.OBJ: _padInitDirSeq, func_80084B78, LIBPAD_PADSEQD_text_108, _dirFailAuto, CardObj_IsTransferActive. */
#include "pe1/card_obj.h"
#include "pe1/psyq_pad_main.h"

int func_80084B78(CardObj *obj);
int CardObj_IsTransferActive(CardObj *obj);
void LIBPAD_PADSEQD_text_108(CardObj *port);

/* LIBPAD hook slots that _padInitDirSeq points at this object's routines. */
extern int (*g_MemCardStateDispatchFn)(CardObj *obj);
extern void (*g_MemCardResponseHandler)(CardObj *port);

void _padInitDirSeq(void) {
    void (*response_handler)(CardObj *);

    g_MemCardStateDispatchFn = func_80084B78;
    g_MemCardIsTransferActiveFn = CardObj_IsTransferActive;
    response_handler = LIBPAD_PADSEQD_text_108;
    g_MemCardResponseHandler = response_handler;
}

void _padCmdParaMode(CardObj *obj, unsigned char value);
void _padSendAtLoadInfo(CardObj *obj);

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
    _padCmdParaMode(obj, 1);
    goto done;

emit_zero:
    asm volatile("" : : "r"(compare));
    _padCmdParaMode(obj, 0);
    goto done;

dispatch:
    if (obj->fn_14 != 0) {
        obj->fn_14(obj);
    } else {
        _padSendAtLoadInfo(obj);
    }

done:
    return 0;
}


int _padRecvAtLoadInfo(CardObj *);
void LIBPAD_PADSEQD_text_108(CardObj *port) {
    int old = port->field_e8;
    int i;
    int mode = port->response_3c[0] >> 4;
    port->field_e8 = mode;
    if (mode == 15)
        port->field_e8 = old;
    else {
        port->output_30[0] = 0;
        port->output_30[1] = port->response_3c[0];
        for (i = 2; i < port->response_index; i++)
            port->output_30[i] = port->response_3c[i];
    }
    if (port->response_3c[1] == 0 && (port->field_46 != 1 || port->fn_14 != 0) &&
        port->pad_50[0] == 0)
        goto reset;
    if (!CardObj_IsTransferActive(port) && !port->saved_command && !port->field_4a &&
        port->field_e8 != old) {
    reset:
        D_8009B728(port);
    }
    port->field_4a = 0;
    if ((u8)(port->field_46 - 2) < 252 && port->response_3c[0] != 0xf3)
        D_8009B728(port);
    {
        register int state = port->field_46;
        register int one;
        register int value asm("$2");
        if (state != 0 && state != 255 && !port->command)
            return;
        one = 1;
        if (state == one)
            goto initial;
        if (state < 2) {
            value = 254;
            asm("" : : "r"(value));
            if (state == 0)
                goto idle;
            goto advance;
        }
        value = 254;
        if (state == value)
            goto terminal;
        if (state == 255)
            return;
        goto advance;
    idle:
        if (!port->field_e8)
            return;
        value = port->field_46;
        port->field_49 = one;
        goto increment;
    initial:
        value = port->field_46;
        port->field_47 = 0;
    increment:
        value++;
        port->field_46 = value;
        return;
    terminal:
        value = 255;
        port->field_46 = value;
        return;
    advance:
        if (port->fn_18)
            value = port->fn_18(port);
        else
            value = _padRecvAtLoadInfo(port);
        port->field_46 += value;
    }
}

/* Psy-Q LIBPAD/PADSEQD.OBJ _dirFailAuto.
 * Provenance: configs/USA/psyq_provenance.json (LIBPAD PADSEQD). */
void _dirFailAuto(CardObj *port) {
    port->field_4c++;
    switch (port->field_46) {
    case 0:
        break;
    case 1:
        if (port->field_4a < 2)
            port->field_4a++;
        else {
            port->field_49 = 2;
            port->field_46 = 255;
        }
        return;
    default:
        if (port->field_4a < 4) {
            port->field_4a++;
            return;
        }
        if (port->field_49)
            D_8009B728(port);
    }
    if (*port->response_3c != 0xf3) {
        port->output_30[0] = 255;
        port->output_30[1] = 0;
        port->field_e8 = 0;
    }
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
