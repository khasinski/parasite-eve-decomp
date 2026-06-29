/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned char u8;

extern void (*g_MemCardObjResetFn)(void *);

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
