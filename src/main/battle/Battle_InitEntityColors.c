/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK Entity_SetActionMode();
extern s8 D_8009CE30;
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(s32 *)&g_PlayerEntity_o)
extern struct { char _[16]; } g_ActiveActor_call_o __asm__("g_ActiveActor");
extern struct { char _[16]; } g_ActiveActor_flags_o __asm__("g_ActiveActor");
#define g_ActiveActor_call (*(void **)&g_ActiveActor_call_o)
#define g_ActiveActor_flags (*(void **)&g_ActiveActor_flags_o)
extern s16 D_8009D298;
extern struct { char _[16]; } D_800B0158_o __asm__("D_800B0158");
#define D_800B0158 (*(u8 *)&D_800B0158_o)
extern struct { char _[16]; } D_800B0159_o __asm__("D_800B0159");
#define D_800B0159 (*(u8 *)&D_800B0159_o)
extern struct { char _[16]; } D_800B015A_o __asm__("D_800B015A");
#define D_800B015A (*(u8 *)&D_800B015A_o)
extern struct { char _[16]; } D_800B0160_o __asm__("D_800B0160");
#define D_800B0160 (*(u8 *)&D_800B0160_o)
extern struct { char _[16]; } D_800B0161_o __asm__("D_800B0161");
#define D_800B0161 (*(u8 *)&D_800B0161_o)
extern struct { char _[16]; } D_800B0162_o __asm__("D_800B0162");
#define D_800B0162 (*(u8 *)&D_800B0162_o)
extern struct { char _[16]; } D_800B0168_o __asm__("D_800B0168");
#define D_800B0168 (*(u8 *)&D_800B0168_o)
extern struct { char _[16]; } D_800B0169_o __asm__("D_800B0169");
#define D_800B0169 (*(u8 *)&D_800B0169_o)
extern struct { char _[16]; } D_800B016A_o __asm__("D_800B016A");
#define D_800B016A (*(u8 *)&D_800B016A_o)
extern struct { char _[16]; } D_800B0170_o __asm__("D_800B0170");
#define D_800B0170 (*(u8 *)&D_800B0170_o)
extern struct { char _[16]; } D_800B0171_o __asm__("D_800B0171");
#define D_800B0171 (*(u8 *)&D_800B0171_o)
extern struct { char _[16]; } D_800B0172_o __asm__("D_800B0172");
#define D_800B0172 (*(u8 *)&D_800B0172_o)
extern struct { char _[16]; } D_800B01A0_o __asm__("D_800B01A0");
#define D_800B01A0 (*(u8 *)&D_800B01A0_o)
extern struct { char _[16]; } D_800B01A1_o __asm__("D_800B01A1");
#define D_800B01A1 (*(u8 *)&D_800B01A1_o)
extern struct { char _[16]; } D_800B01A2_o __asm__("D_800B01A2");
#define D_800B01A2 (*(u8 *)&D_800B01A2_o)
extern struct { char _[16]; } D_800B01A8_o __asm__("D_800B01A8");
#define D_800B01A8 (*(u8 *)&D_800B01A8_o)
extern struct { char _[16]; } D_800B01A9_o __asm__("D_800B01A9");
#define D_800B01A9 (*(u8 *)&D_800B01A9_o)
extern struct { char _[16]; } D_800B01AA_o __asm__("D_800B01AA");
#define D_800B01AA (*(u8 *)&D_800B01AA_o)
extern struct { char _[16]; } D_800B01B0_o __asm__("D_800B01B0");
#define D_800B01B0 (*(u8 *)&D_800B01B0_o)
extern struct { char _[16]; } D_800B01B1_o __asm__("D_800B01B1");
#define D_800B01B1 (*(u8 *)&D_800B01B1_o)
extern struct { char _[16]; } D_800B01B2_o __asm__("D_800B01B2");
#define D_800B01B2 (*(u8 *)&D_800B01B2_o)
extern struct { char _[16]; } D_800B01B8_o __asm__("D_800B01B8");
#define D_800B01B8 (*(u8 *)&D_800B01B8_o)
extern struct { char _[16]; } D_800B01B9_o __asm__("D_800B01B9");
#define D_800B01B9 (*(u8 *)&D_800B01B9_o)
extern struct { char _[16]; } D_800B01BA_o __asm__("D_800B01BA");
#define D_800B01BA (*(u8 *)&D_800B01BA_o)

void Battle_InitEntityColors(void) {
    s32 flags;

    Entity_SetActionMode(g_PlayerEntity, M2C_FIELD(g_ActiveActor_call, u8 *, 0x12));
    flags = M2C_FIELD(g_ActiveActor_flags, s32 *, 0x4C);
    D_8009CE30 = 0;
    M2C_FIELD(g_ActiveActor_flags, s32 *, 0x4C) = flags & ~0x2000;
    D_8009D298 = 0;
    D_800B0158 = 0xFF;
    D_800B0159 = 0x3D;
    D_800B015A = 0x81;
    D_800B0160 = 0x83;
    D_800B0161 = 0x13;
    D_800B0162 = 1;
    D_800B0168 = 0xFF;
    D_800B0169 = 0x3D;
    D_800B016A = 0x81;
    D_800B0170 = 0x83;
    D_800B0171 = 0x13;
    D_800B0172 = 1;
    D_800B01A0 = 0xFF;
    D_800B01A1 = 0x3D;
    D_800B01A2 = 0x81;
    D_800B01A8 = 0x83;
    D_800B01A9 = 0x13;
    D_800B01AA = 1;
    D_800B01B0 = 0xFF;
    D_800B01B1 = 0x3D;
    D_800B01B2 = 0x81;
    D_800B01B8 = 0x83;
    D_800B01B9 = 0x13;
    D_800B01BA = 1;
}
