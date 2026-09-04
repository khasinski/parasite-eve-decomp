#include "common.h"

typedef struct RoomOrbitInitWords {
    int words[8];
} RoomOrbitInitWords;

#ifdef ROOMLIB_INIT_ORBIT_RENDER_COMPACT_GLOBALS
#define ROOMLIB_ORBIT_50 D_80190080
#define ROOMLIB_ORBIT_51 D_80190081
#define ROOMLIB_ORBIT_52 D_80190082
#define ROOMLIB_ORBIT_54 D_80190084
#define ROOMLIB_ORBIT_55 D_80190085
#define ROOMLIB_ORBIT_56 D_80190086
#define ROOMLIB_ORBIT_58 D_80190088
#define ROOMLIB_ORBIT_5A D_8019008A
#define ROOMLIB_ORBIT_70 D_80190090
#define ROOMLIB_ORBIT_71 D_80190091
#define ROOMLIB_ORBIT_72 D_80190092
#define ROOMLIB_ORBIT_74 D_80190094
#define ROOMLIB_ORBIT_75 D_80190095
#define ROOMLIB_ORBIT_76 D_80190096
#define ROOMLIB_ORBIT_78 D_80190098
#define ROOMLIB_ORBIT_7A D_8019009A
#define ROOMLIB_ORBIT_90 D_801900A0
#define ROOMLIB_ORBIT_91 D_801900A1
#define ROOMLIB_ORBIT_92 D_801900A2
#define ROOMLIB_ORBIT_94 D_801900A4
#define ROOMLIB_ORBIT_95 D_801900A5
#define ROOMLIB_ORBIT_96 D_801900A6
#define ROOMLIB_ORBIT_98 D_801900A8
#define ROOMLIB_ORBIT_9A D_801900AA
#define ROOMLIB_ORBIT_A0 D_801900B0
#define ROOMLIB_ORBIT_A1 D_801900B1
#define ROOMLIB_ORBIT_A2 D_801900B2
#define ROOMLIB_ORBIT_A4 D_801900B4
#define ROOMLIB_ORBIT_A5 D_801900B5
#define ROOMLIB_ORBIT_A6 D_801900B6
#define ROOMLIB_ORBIT_A8 D_801900B8
#define ROOMLIB_ORBIT_AA D_801900BA
#else
#define ROOMLIB_ORBIT_50 D_80194350
#define ROOMLIB_ORBIT_51 D_80194351
#define ROOMLIB_ORBIT_52 D_80194352
#define ROOMLIB_ORBIT_54 D_80194354
#define ROOMLIB_ORBIT_55 D_80194355
#define ROOMLIB_ORBIT_56 D_80194356
#define ROOMLIB_ORBIT_58 D_80194358
#define ROOMLIB_ORBIT_5A D_8019435A
#define ROOMLIB_ORBIT_70 D_80194370
#define ROOMLIB_ORBIT_71 D_80194371
#define ROOMLIB_ORBIT_72 D_80194372
#define ROOMLIB_ORBIT_74 D_80194374
#define ROOMLIB_ORBIT_75 D_80194375
#define ROOMLIB_ORBIT_76 D_80194376
#define ROOMLIB_ORBIT_78 D_80194378
#define ROOMLIB_ORBIT_7A D_8019437A
#define ROOMLIB_ORBIT_90 D_80194390
#define ROOMLIB_ORBIT_91 D_80194391
#define ROOMLIB_ORBIT_92 D_80194392
#define ROOMLIB_ORBIT_94 D_80194394
#define ROOMLIB_ORBIT_95 D_80194395
#define ROOMLIB_ORBIT_96 D_80194396
#define ROOMLIB_ORBIT_98 D_80194398
#define ROOMLIB_ORBIT_9A D_8019439A
#define ROOMLIB_ORBIT_A0 D_801943A0
#define ROOMLIB_ORBIT_A1 D_801943A1
#define ROOMLIB_ORBIT_A2 D_801943A2
#define ROOMLIB_ORBIT_A4 D_801943A4
#define ROOMLIB_ORBIT_A5 D_801943A5
#define ROOMLIB_ORBIT_A6 D_801943A6
#define ROOMLIB_ORBIT_A8 D_801943A8
#define ROOMLIB_ORBIT_AA D_801943AA
#endif

extern unsigned char ROOMLIB_ORBIT_50;
extern unsigned char ROOMLIB_ORBIT_51;
extern unsigned char ROOMLIB_ORBIT_52;
extern unsigned char ROOMLIB_ORBIT_54;
extern unsigned char ROOMLIB_ORBIT_55;
extern unsigned char ROOMLIB_ORBIT_56;
extern short ROOMLIB_ORBIT_58;
extern short ROOMLIB_ORBIT_5A;
extern unsigned char ROOMLIB_ORBIT_70;
extern unsigned char ROOMLIB_ORBIT_71;
extern unsigned char ROOMLIB_ORBIT_72;
extern unsigned char ROOMLIB_ORBIT_74;
extern unsigned char ROOMLIB_ORBIT_75;
extern unsigned char ROOMLIB_ORBIT_76;
extern short ROOMLIB_ORBIT_78;
extern short ROOMLIB_ORBIT_7A;
extern unsigned char ROOMLIB_ORBIT_90;
extern unsigned char ROOMLIB_ORBIT_91;
extern unsigned char ROOMLIB_ORBIT_92;
extern unsigned char ROOMLIB_ORBIT_94;
extern unsigned char ROOMLIB_ORBIT_95;
extern unsigned char ROOMLIB_ORBIT_96;
extern short ROOMLIB_ORBIT_98;
extern short ROOMLIB_ORBIT_9A;
extern unsigned char ROOMLIB_ORBIT_A0;
extern unsigned char ROOMLIB_ORBIT_A1;
extern unsigned char ROOMLIB_ORBIT_A2;
extern unsigned char ROOMLIB_ORBIT_A4;
extern unsigned char ROOMLIB_ORBIT_A5;
extern unsigned char ROOMLIB_ORBIT_A6;
extern short ROOMLIB_ORBIT_A8;
extern short ROOMLIB_ORBIT_AA;

void func_800C2B40(void *state);
void *func_8006DC18(int size);
int func_800C66C8(void *object, int message, void *state);

void ROOMLIB_INIT_ORBIT_RENDER_NAME(char *object, void *unused, char *state) {
    char *owner;
    void *allocation;
    RoomOrbitInitWords *source;
    register int callObject asm("$4");
    register int value asm("$2");
    int value128a;
    int value128b;
    int value32;
    register int value1 asm("$7");
    register int valueNeg42 asm("$5");

    func_800C2B40(state);
    owner = *(char **)(object + 8);
    *(char **)state = owner;
    source = *(RoomOrbitInitWords **)(owner + 0x238);
    *(RoomOrbitInitWords *)(state + 4) = *source;
    allocation = func_8006DC18(0x24);

    callObject = (int)object;
    *(void **)(state + 0x24) = allocation;
    value = -0x29;
    value128a = 0x80;
    value128b = 0x80;
    value32 = 0x20;
    value1 = 1;
    valueNeg42 = -0x2A;
    PE1_COMPILER_LAUNDER(value);
    PE1_COMPILER_LAUNDER(value128a);
    PE1_COMPILER_LAUNDER(value128b);
    PE1_COMPILER_LAUNDER(value32);
    PE1_COMPILER_LAUNDER(value1);
    PE1_COMPILER_LAUNDER(valueNeg42);
    ROOMLIB_ORBIT_78 = value;
    value = 0x60;
    ROOMLIB_ORBIT_90 = value;
    value = 0x40;
    asm volatile("" : "=r"(state) : "0"(state), "r"(value));
    state += 4;
    PE1_COMPILER_USE(state);
    ROOMLIB_ORBIT_98 = valueNeg42;
    ROOMLIB_ORBIT_A8 = valueNeg42;
    ROOMLIB_ORBIT_58 = valueNeg42;
    ROOMLIB_ORBIT_7A = value128a;
    ROOMLIB_ORBIT_9A = value128a;
    ROOMLIB_ORBIT_5A = value128a;
    ROOMLIB_ORBIT_74 = 0;
    ROOMLIB_ORBIT_75 = 0;
    ROOMLIB_ORBIT_70 = value128b;
    ROOMLIB_ORBIT_71 = value128b;
    ROOMLIB_ORBIT_72 = value128b;
    ROOMLIB_ORBIT_76 = 0;
    ROOMLIB_ORBIT_94 = value32;
    ROOMLIB_ORBIT_95 = value1;
    ROOMLIB_ORBIT_91 = value;
    ROOMLIB_ORBIT_92 = value;
    ROOMLIB_ORBIT_96 = 0;
    ROOMLIB_ORBIT_A4 = value32;
    ROOMLIB_ORBIT_A5 = value1;
    ROOMLIB_ORBIT_AA = 0;
    ROOMLIB_ORBIT_A0 = 0;
    ROOMLIB_ORBIT_A1 = 0;
    ROOMLIB_ORBIT_A2 = 0;
    ROOMLIB_ORBIT_A6 = 0;
    ROOMLIB_ORBIT_54 = 0;
    ROOMLIB_ORBIT_55 = 0;
    ROOMLIB_ORBIT_50 = value128b;
    ROOMLIB_ORBIT_51 = value128b;
    ROOMLIB_ORBIT_52 = value128b;
    ROOMLIB_ORBIT_56 = 0;
    func_800C66C8((void *)callObject, 0x592, state);
}
