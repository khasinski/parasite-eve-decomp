typedef unsigned char u8;

void CardObj_EmitCommand43(void *arg0, int arg1);
void CardObj_EmitReadCommandForState(void);

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
