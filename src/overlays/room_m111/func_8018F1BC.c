typedef signed short s16;
typedef unsigned char u8;

typedef struct RoomM111Words8 {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;
} RoomM111Words8;

extern void *D_800B0E64;
extern s16 D_80190190;
extern s16 D_80190192;
extern u8 D_80190188;
extern u8 D_80190189;
extern u8 D_8019018A;
extern u8 D_8019018C;
extern u8 D_8019018D;
extern u8 D_8019018E;
extern void *D_80190194;

extern void func_800C2B40(void *arg0);
extern void *func_8006DC18(int type);
extern void *func_8006E498(void *arg0, int arg1);

void func_8018F1BC(char *arg0, void *arg1, char *state) {
    RoomM111Words8 *src;
    int *table;

    func_800C2B40(state);
    table = *(int **)(arg0 + 8);
    *(int **)(state + 0x0) = table;
    src = *(RoomM111Words8 **)((char *)table + 0x238);
    *(RoomM111Words8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x2A) = 0;
    *(s16 *)(state + 0x2C) = 0;
    *(s16 *)(state + 0x28) = 0x1E;
    *(void **)(state + 0x24) = func_8006DC18(0x23);

    D_80190190 = -0x12C;
    D_80190192 = 0x80;
    D_8019018C = 0;
    D_8019018D = 0;
    D_80190188 = 0x80;
    D_80190189 = 0x80;
    D_8019018A = 0x80;
    D_8019018E = 0;
    D_80190194 = func_8006E498(D_800B0E64, 0xCB8704);
}
