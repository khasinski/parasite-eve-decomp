typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct RoomM087Words8 {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} RoomM087Words8;

extern u8 D_80193628;
extern u8 D_80193629;
extern u8 D_8019362A;
extern u8 D_8019362C;
extern u8 D_8019362D;
extern u8 D_8019362E;
extern s16 D_80193630;
extern s16 D_80193632;
extern u8 D_80193638;
extern u8 D_80193639;
extern u8 D_8019363A;
extern u8 D_8019363C;
extern u8 D_8019363D;
extern u8 D_8019363E;
extern s16 D_80193640;
extern s16 D_80193642;

extern void func_800C2B40(void *arg0);
extern s32 *func_800C2B28(int arg0);
extern void *func_8006DC18(int type);

void func_8018F394(char *obj, void *arg1, char *state) {
    char *owner;
    RoomM087Words8 *src;

    func_800C2B40(state);
    *(void **)(state + 0x2C) = func_8006DC18(0xA);

    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    src = *(RoomM087Words8 **)(owner + 0x238);
    *(RoomM087Words8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x26) = 0;
    *(s16 *)(state + 0x28) = 0;
    *(s16 *)(state + 0x24) = *func_800C2B28(6);

    D_80193640 = -0x64;
    D_8019362C = 4;
    D_8019362D = 1;
    D_8019363C = 0;
    D_8019363D = 0;
    D_80193642 = 0x80;
    D_80193638 = 0x80;
    D_80193639 = 0x80;
    D_8019363A = 0x80;
    D_8019363E = 0;
    D_80193630 = 0x32;
    D_80193632 = 0x80;
    D_80193628 = 0x80;
    D_80193629 = 0x80;
    D_8019362A = 0x80;
    D_8019362E = 0;
}
