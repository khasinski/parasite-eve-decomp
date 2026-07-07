
typedef unsigned int u32;
typedef unsigned char u8;

extern u8 D_8009B558[];
extern u8 D_8009B588[];
extern u32 D_8009B624[];

void Util_Copy8(u8 *dst, u8 *src);

void CdRom_ProcessEventByte(int event, u8 *data) {
    register u8 *data_reg asm("$6");
    register u8 *state asm("$4");
    register u8 value asm("$5");
    u8 *src;
    int index;

    data_reg = data;
    event &= 0xFF;
    if (event == 5) {
        goto event_five;
    }

    index = D_8009B624[D_8009B558[0]] - 1;
    if (index < 0) {
        return;
    }
    goto process;

event_five:
    index = 0;

process:
    src = data_reg + index;
    value = src[0];
    asm volatile("" : : "r"(value));
    state = D_8009B588;
    asm volatile("" : "=r"(state) : "0"(state));
    state[0] = value >> 7;
    state[1] = (value >> 6) & 1;
    state[2] = (value >> 5) & 1;
    state[3] = (value >> 1) & 1;
    asm volatile("" : "=r"(state) : "0"(state));
    state[-0x1C] = value;
    Util_Copy8(state - 0x24, data_reg);
}
