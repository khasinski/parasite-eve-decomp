typedef int s32;

typedef struct {
    char pad[0xC];
    void (*callback)(void *);
} OverlayCallback;

s32 func_80181D4C(OverlayCallback *arg0) {
    arg0->callback(arg0);
    return 0;
}
