typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Task_ConvertSecondsToHMS(void *arg0) {
    *M2C_FIELD(arg0, s32 **, 4) = *M2C_FIELD(arg0, s32 **, 0) / 216000;
    *M2C_FIELD(arg0, s32 **, 8) = (*M2C_FIELD(arg0, s32 **, 0) % 216000) / 3600;
    *M2C_FIELD(arg0, s32 **, 0xC) = (*M2C_FIELD(arg0, s32 **, 0) % 3600) / 60;
    return 1;
}
