/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_palette.h"
#include "pe1/battle.h"

#define PALETTE_BYTE(name) (*(s8 *)&(name))
#define PALETTE_WORD(name) (*(u32 *)&(name))
#define PALETTE_POINTER(name) (*(BattleEntity **)&(name))

void Battle_SetupPlayerPalette(void) {
    int c2;
    int c3;
    int c4;
    int c5;
    int c6;
    register int c7 asm("$7");
    u8 *flags;

    c6 = 0x82;
    c5 = 0x36;
    c4 = 0x4A;
    c2 = 0xFF;
    asm("" : : "r"(c2));
    c3 = 0x3B;
    c7 = 0x3D;
    asm("" : : "r"(c2), "r"(c7));

    PALETTE_BYTE(D_800B0135) = c6;
    PALETTE_BYTE(D_800B0145) = c6;
    PALETTE_BYTE(D_800B017D) = c6;
    PALETTE_BYTE(D_800B018D) = c6;

    c6 = 0x81;
    PALETTE_BYTE(D_800B0136) = c5;
    PALETTE_BYTE(D_800B0146) = c5;
    PALETTE_BYTE(D_800B017E) = c5;
    PALETTE_BYTE(D_800B018E) = c5;

    c5 = 0x83;
    PALETTE_BYTE(D_800B013C) = c4;
    PALETTE_BYTE(D_800B014C) = c4;
    PALETTE_BYTE(D_800B0184) = c4;
    PALETTE_BYTE(D_800B0194) = c4;

    c4 = 0x13;
    PALETTE_BYTE(D_800B013E) = c3;
    PALETTE_BYTE(D_800B014E) = c3;
    PALETTE_BYTE(D_800B0186) = c3;
    PALETTE_BYTE(D_800B0196) = c3;

    c3 = 1;
    PALETTE_BYTE(D_800B0134) = 0;
    PALETTE_BYTE(D_800B013D) = c2;
    PALETTE_BYTE(D_800B0144) = 0;
    PALETTE_BYTE(D_800B014D) = c2;
    PALETTE_BYTE(D_800B017C) = 0;
    PALETTE_BYTE(D_800B0185) = c2;
    PALETTE_BYTE(D_800B018C) = 0;
    PALETTE_BYTE(D_800B0195) = c2;

    PALETTE_BYTE(D_800B0158) = c2;
    PALETTE_BYTE(D_800B0159) = c7;
    PALETTE_BYTE(D_800B015A) = c6;
    PALETTE_BYTE(D_800B0160) = c5;
    PALETTE_BYTE(D_800B0161) = c4;
    PALETTE_BYTE(D_800B0162) = c3;
    PALETTE_BYTE(D_800B0168) = c2;
    PALETTE_BYTE(D_800B0169) = c7;
    PALETTE_BYTE(D_800B016A) = c6;
    PALETTE_BYTE(D_800B0170) = c5;
    PALETTE_BYTE(D_800B0171) = c4;
    PALETTE_BYTE(D_800B0172) = c3;
    PALETTE_BYTE(D_800B01A0) = c2;
    PALETTE_BYTE(D_800B01A1) = c7;
    PALETTE_BYTE(D_800B01A2) = c6;
    PALETTE_BYTE(D_800B01A8) = c5;
    PALETTE_BYTE(D_800B01A9) = c4;
    PALETTE_BYTE(D_800B01AA) = c3;
    PALETTE_BYTE(D_800B01B0) = c2;
    PALETTE_BYTE(D_800B01B1) = c7;
    PALETTE_BYTE(D_800B01B2) = c6;
    PALETTE_BYTE(D_800B01B8) = c5;
    PALETTE_BYTE(D_800B01B9) = c4;
    PALETTE_BYTE(D_800B01BA) = c3;

    Battle_FlushScriptSounds();
    D_8009D2EC = 0;
    D_8009D2A0 = 0;
    PALETTE_POINTER(D_8009D254)->actionCheckFn = (void *)PALETTE_WORD(D_800915E0);
    *(u32 *)&D_8009D1A0_write = PALETTE_WORD(D_8009D1A0_read) & -3;
    *(u32 *)&D_8009D2E8_write = PALETTE_WORD(D_8009D2E8_read) & -0x11;
    flags = &PALETTE_BYTE(D_800B0CE6);
    *flags |= 2;
    BattleCmd_SyncActiveAmmo();
}
