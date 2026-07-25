typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct RoomM107Words8 {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} RoomM107Words8;

extern s32 D_800B0E64;
extern u8 D_80193720;
extern u8 D_80193721;
extern u8 D_80193722;
extern u8 D_80193724;
extern u8 D_80193725;
extern u8 D_80193726;
extern s16 D_80193728;
extern s16 D_8019372A;
extern s32 D_8019372C;

extern void func_800C2B40(void *arg0);
extern s32 func_8006DC18(s32 type);
extern s32 func_8006E498(s32 arg0, s32 arg1);

s32 func_8018F394(char *obj, s32 arg1, char *state) {
    char *owner;
    RoomM107Words8 *src;

    func_800C2B40(state);
    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    src = *(RoomM107Words8 **)(owner + 0x238);
    *(RoomM107Words8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x2A) = 0;
    *(s16 *)(state + 0x2C) = 0;
    *(s16 *)(state + 0x28) = 0x1E;
    *(s32 *)(state + 0x24) = func_8006DC18(0x23);

    D_80193728 = -0x12C;
    D_8019372A = 0x80;
    D_80193724 = 0;
    D_80193725 = 0;
    D_80193720 = 0x80;
    D_80193721 = 0x80;
    D_80193722 = 0x80;
    D_80193726 = 0;
    return D_8019372C = func_8006E498(D_800B0E64, 0xCB8704);
}
