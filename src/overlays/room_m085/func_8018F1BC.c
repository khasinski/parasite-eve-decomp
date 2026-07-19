typedef signed short s16;
typedef unsigned char u8;

typedef struct RoomM085Words8 {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;
} RoomM085Words8;

extern s16 D_80190090;
extern s16 D_80190092;
extern u8 D_80190088;
extern u8 D_80190089;
extern u8 D_8019008A;
extern u8 D_8019008C;
extern u8 D_8019008D;
extern u8 D_8019008E;
extern u8 D_80190098;
extern u8 D_80190099;
extern u8 D_8019009A;
extern u8 D_8019009C;
extern u8 D_8019009D;
extern u8 D_8019009E;
extern s16 D_801900A0;
extern s16 D_801900A2;

extern void func_800C2B40(void *arg0);
extern void *func_8006DC18(int type);
extern int *func_800C2B28(int index);

void func_8018F1BC(char *arg0, void *arg1, char *state) {
    RoomM085Words8 *src;
    int *table;

    func_800C2B40(state);
    *(void **)(state + 0x2C) = func_8006DC18(0xA);
    table = *(int **)(arg0 + 8);
    *(int **)(state + 0x0) = table;
    src = *(RoomM085Words8 **)((char *)table + 0x238);
    *(RoomM085Words8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x26) = 0;
    *(s16 *)(state + 0x28) = 0;
    *(s16 *)(state + 0x24) = *func_800C2B28(6);

    D_801900A0 = -0x64;
    D_8019008C = 4;
    D_8019008D = 1;
    D_8019009C = 0;
    D_8019009D = 0;
    D_801900A2 = 0x80;
    D_80190098 = 0x80;
    D_80190099 = 0x80;
    D_8019009A = 0x80;
    D_8019009E = 0;
    D_80190090 = 0x32;
    D_80190092 = 0x80;
    D_80190088 = 0x80;
    D_80190089 = 0x80;
    D_8019008A = 0x80;
    D_8019008E = 0;
}
