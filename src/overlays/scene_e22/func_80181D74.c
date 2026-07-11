typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    char pad0[0x8];
    u8 *data;
    void (*callback)(void *);
    char pad10[0x6];
    s8 state_id;
    s8 range_value;
} Overlay189CallbackState;

extern void D_80190DCC(void *);

void func_80181D74(Overlay189CallbackState *arg0) {
    s8 state_id;
    s8 range_value;
    u8 *data;
    u16 range_start;
    u16 range_end;

    state_id = arg0->state_id;
    if (state_id < 0 || state_id == arg0->data[0xE]) {
        range_value = arg0->range_value;
        if (range_value < 0) {
            arg0->callback = D_80190DCC;
        } else {
            data = arg0->data;
            range_start = *(u16 *)(data + 0x1A);
            range_end = *(u16 *)(data + 0x16);
            if (range_start < range_value && range_end >= range_value) {
                arg0->callback = D_80190DCC;
            }
        }
    }
}
