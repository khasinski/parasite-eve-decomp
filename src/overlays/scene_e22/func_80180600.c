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
    char pad18[0x9C];
    u8 refresh_callback;
} Overlay189PrimaryCallbackState;

void func_8018FBD8(u8 *data, void (**callback)(void *), void *scratch);
extern void D_8018F690(void *);

void func_80180600(Overlay189PrimaryCallbackState *arg0) {
    s8 state_id;
    s8 range_value;
    u8 *data;
    u16 range_start;
    u16 range_end;

    if (arg0->refresh_callback != 0) {
        func_8018FBD8(arg0->data, &arg0->callback, (void *)0x1F800000);
    }

    state_id = arg0->state_id;
    if (state_id < 0 || state_id == arg0->data[0xE]) {
        range_value = arg0->range_value;
        if (range_value < 0) {
            arg0->callback = D_8018F690;
        } else {
            data = arg0->data;
            range_start = *(u16 *)(data + 0x1A);
            range_end = *(u16 *)(data + 0x16);
            if (range_value >= range_start && range_end >= range_value) {
                arg0->callback = D_8018F690;
            }
        }
    }
}
