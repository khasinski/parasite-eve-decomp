typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct RoomM395Words8 {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} RoomM395Words8;

extern u8 D_80194548;
extern u8 D_80194549;
extern u8 D_8019454A;
extern u8 D_8019454C;
extern u8 D_8019454D;
extern u8 D_8019454E;
extern s16 D_80194550;
extern s16 D_80194552;
extern u8 D_80194558;
extern u8 D_80194559;
extern u8 D_8019455A;
extern u8 D_8019455C;
extern u8 D_8019455D;
extern u8 D_8019455E;
extern s16 D_80194560;
extern s16 D_80194562;

extern void func_800C2B40(void *arg0);
extern s32 *func_800C2B28(int arg0);
extern void *func_8006DC18(int type);

void func_8018F3A8(char *obj, void *arg1, char *state) {
    char *owner;
    RoomM395Words8 *src;

    func_800C2B40(state);
    *(void **)(state + 0x2C) = func_8006DC18(0xA);

    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    src = *(RoomM395Words8 **)(owner + 0x238);
    *(RoomM395Words8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x26) = 0;
    *(s16 *)(state + 0x28) = 0;
    *(s16 *)(state + 0x24) = *func_800C2B28(6);

    D_80194560 = -0x64;
    D_8019454C = 4;
    D_8019454D = 1;
    D_8019455C = 0;
    D_8019455D = 0;
    D_80194562 = 0x80;
    D_80194558 = 0x80;
    D_80194559 = 0x80;
    D_8019455A = 0x80;
    D_8019455E = 0;
    D_80194550 = 0x32;
    D_80194552 = 0x80;
    D_80194548 = 0x80;
    D_80194549 = 0x80;
    D_8019454A = 0x80;
    D_8019454E = 0;
}
