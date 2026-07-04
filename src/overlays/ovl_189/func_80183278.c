typedef struct {
    char pad0[0x8];
    unsigned char *data;
    void (*callback)(void *);
    char pad10[0x6];
    signed char state_id;
} OverlayState;

extern void D_80192290(void *);

void func_80183278(OverlayState *arg0) {
    if (arg0->data[0xE] == arg0->state_id) {
        arg0->callback = D_80192290;
    }
}
