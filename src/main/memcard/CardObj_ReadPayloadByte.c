typedef unsigned char u8;

typedef struct {
    char pad00[0x28];
    u8 *field28;
    u8 *field2C;
    char pad30[4];
    u8 field34;
    u8 field35;
    u8 field36;
    char pad37[0xE];
    u8 field45;
    char pad46[0x11];
    u8 field57[6];
} Unk800847B0;

int CardObj_ReadPayloadByte(Unk800847B0 *arg0) {
    int index;
    int mode;
    u8 *ptr;

    index = arg0->field45 - 3;
    mode = arg0->field36;
    if (mode == 0) {
        goto mode0;
    }
    if (mode == 0x4D) {
        goto mode4D;
    }
    goto other;

mode0:
    if (index < 6) {
        if (arg0->field57[index] == 0) {
            return 0;
        }
    }

    if (index >= arg0->field34) {
        return 0;
    }
    ptr = arg0->field28 + index;

load:
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    return *ptr;

mode4D:
    if (index < arg0->field35) {
        ptr = arg0->field2C + index;
        goto load;
    }
    return 0xFF;

other:
    if (index >= arg0->field35) {
        return 0;
    }
    return arg0->field2C[index];
}
