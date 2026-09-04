#include "room_lib.h"

extern int *func_800C2B28(int index);

extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_50;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_51;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_52;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_54;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_55;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_56;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_58;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_5A;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_60;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_61;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_62;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_64;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_65;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_66;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_68;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_6A;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_70;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_71;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_72;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_74;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_75;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_76;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_78;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_7A;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_80;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_81;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_82;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_84;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_85;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_86;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_88;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_8A;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_90;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_91;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_92;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_94;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_95;
extern volatile unsigned char ROOMFX_INIT_RENDER_CONFIGS_96;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_98;
extern volatile short ROOMFX_INIT_RENDER_CONFIGS_9A;

void ROOMFX_INIT_RENDER_CONFIGS_NAME(RoomEnt *arg0, int unused, void *arg2) {
    char *state = arg2;
    RoomEnt *self = arg0;

    func_800C2B40(state);
    RW32(state, 0x10) = (int)func_8006DC18(0x28);
    RW16(state, 0x6) = 0;
    RW16(state, 0x8) = 0;
    RW32(state, 0x0) = RW32(self, 0x8);
    RW16(state, 0xA) = *func_800C2B28(1);
    RW16(state, 0xC) = *func_800C2B28(2);
    RW16(state, 0x4) = *func_800C2B28(3);

    ROOMFX_INIT_RENDER_CONFIGS_78 = 0xA;
    ROOMFX_INIT_RENDER_CONFIGS_68 = 0x64;
    ROOMFX_INIT_RENDER_CONFIGS_84 = 0x20;
    ROOMFX_INIT_RENDER_CONFIGS_88 = 0xA0;
    {
        int value24 = 0x24;
        int value1 = 1;
        int value80arg = 0x80;
        register unsigned char value80 asm("$3") = 0x80;
        int value6e = 0x6E;
        ROOMFX_INIT_RENDER_CONFIGS_74 = value24;
        ROOMFX_INIT_RENDER_CONFIGS_75 = value1;
        ROOMFX_INIT_RENDER_CONFIGS_7A = value80arg;
        ROOMFX_INIT_RENDER_CONFIGS_70 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_71 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_72 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_76 = 0;
        ROOMFX_INIT_RENDER_CONFIGS_64 = 0;
        ROOMFX_INIT_RENDER_CONFIGS_65 = 0;
        ROOMFX_INIT_RENDER_CONFIGS_6A = value80arg;
        ROOMFX_INIT_RENDER_CONFIGS_60 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_61 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_62 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_66 = 0;
        ROOMFX_INIT_RENDER_CONFIGS_85 = value1;
        ROOMFX_INIT_RENDER_CONFIGS_8A = value80arg;
        ROOMFX_INIT_RENDER_CONFIGS_80 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_81 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_82 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_86 = 0;
        ROOMFX_INIT_RENDER_CONFIGS_94 = value24;
        ROOMFX_INIT_RENDER_CONFIGS_95 = value1;
        ROOMFX_INIT_RENDER_CONFIGS_98 = value6e;
        ROOMFX_INIT_RENDER_CONFIGS_9A = value80arg;
        ROOMFX_INIT_RENDER_CONFIGS_90 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_91 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_92 = value80;
        ROOMFX_INIT_RENDER_CONFIGS_96 = 0;
        ROOMFX_INIT_RENDER_CONFIGS_58 = value6e;
        ROOMFX_INIT_RENDER_CONFIGS_5A = 0x30;
        ROOMFX_INIT_RENDER_CONFIGS_54 = value24;
        ROOMFX_INIT_RENDER_CONFIGS_55 = value1;
        ROOMFX_INIT_RENDER_CONFIGS_50 = 0xA0;
        ROOMFX_INIT_RENDER_CONFIGS_51 = 0xA0;
        ROOMFX_INIT_RENDER_CONFIGS_52 = 0xA0;
        ROOMFX_INIT_RENDER_CONFIGS_56 = 0;
    }
}
