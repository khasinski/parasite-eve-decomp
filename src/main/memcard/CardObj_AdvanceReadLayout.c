/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

int CardObj_CalcReadPayloadSize(void *arg0);
int CardObj_BeginReadTransfer(void *arg0, void *arg1);

int CardObj_AdvanceReadLayout(void *arg0) {
    u8 *obj = arg0;
    u8 *ptr;
    volatile u8 *vptr;
    int temp;
    int pos;

    switch (obj[0x46]) {
    case 2:
        obj[0xE3] = ((u8 *)*(void **)(obj + 0x3C))[3];
        obj[0xE4] = ((u8 *)*(void **)(obj + 0x3C))[4];
        *(u16 *)(obj + 0xE6) = 0;
        obj[0xE9] = ((u8 *)*(void **)(obj + 0x3C))[5];
        obj[0xEA] = ((u8 *)*(void **)(obj + 0x3C))[6];
        *(u32 *)(obj + 0xEC) = 0;
        goto success;

    case 3:
        vptr = *(void **)(obj + 0x3C);
        temp = vptr[4];
        pos = vptr[5];
        obj[0x47] = 0;
        *(u16 *)(obj + 0xE6) = (temp << 8) + pos;
        goto success;

    case 4:
        {
            int accum;
            register int count asm("$2");
            int field;

            accum = *(u32 *)(obj + 0xEC);
            count = obj[0x47];
            field = *(int *)(obj + 0x3C);
            count += 1;
            field = *(u8 *)(field + 4);
            accum += 8;
            obj[0x47] = count;
            field += 3;
            field &= 0x1FC;
            accum += field;
            *(u32 *)(obj + 0xEC) = accum;
            if (((u8)count < obj[0xEA]) == 0) {
                goto do_call;
            }
        }
        goto fail;
    }

    goto success;

fail:
    return 0;

do_call:
    pos = CardObj_CalcReadPayloadSize(obj);
    if (pos >= 0x81) {
        obj[0x46] = 0xFE;
        obj[0x49] = 2;
        goto fail;
    }
    obj[0x46] = 0xFF;
    CardObj_BeginReadTransfer(obj, obj + 0x63);
    obj[0x46] = 2;
    goto fail;

success:
    return 1;
}
