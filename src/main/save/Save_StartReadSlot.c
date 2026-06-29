typedef unsigned char u8;
typedef unsigned short u16;

extern u8 g_MemCardPortStates[];
extern u8 g_MemCardFileBuffer[];
extern void *g_MemCardActiveState;
extern int g_MemCardActiveBytesRemaining;

void bzero(void *dst, int len);
void MemCard_CloseAll(void);

int Save_StartReadSlot(int port, int slot) {
    register u8 *buffer asm("$4");
    u8 *state;
    int saved_slot;

    state = &g_MemCardPortStates[port * 0x418];
    saved_slot = slot;
    if (state[0] == 1) {
        MemCard_CloseAll();
        buffer = g_MemCardFileBuffer;
        *(u16 *)(state + 0x14) = 0x2000;
        *(u16 *)(state + 0x16) = 10;
        state[7] = 2;
        state[1] = 1;
        state[0xB] = 5;
        *(void **)(state + 0x18) = buffer;
        state[3] = saved_slot;
        g_MemCardActiveState = state;
        g_MemCardActiveBytesRemaining = 0x2000;
        bzero(buffer, 0x2000);
    }
    return 0;
}
