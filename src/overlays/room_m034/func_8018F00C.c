#include "../room_lib/room_lib.h"

typedef unsigned char u8;
typedef unsigned short u16;

extern char D_8018FF98;
extern u8 D_80190054;
extern u8 D_80190055;
extern u8 D_80190056;
extern u16 D_80190058;
extern u16 D_8019005A;
extern u8 D_80190060;
extern u8 D_80190061;
extern u8 D_80190062;
extern u8 D_80190064;
extern u8 D_80190065;
extern u8 D_80190066;
extern u16 D_80190068;
extern u16 D_8019006A;

int func_8018F00C(void) {
    *FieldEng_GetSlot() = &D_8018FF98;
    D_80190054 = 0x20;
    D_80190055 = 3;
    D_80190064 = 0x2B;
    D_80190065 = 2;
    D_8019006A = 0x80;
    D_80190058 = 0;
    D_8019005A = 0;
    D_80190056 = 0;
    D_80190068 = 0;
    D_80190060 = 0x80;
    D_80190061 = 0x80;
    D_80190062 = 0x80;
    D_80190066 = 0;
    return 0;
}
