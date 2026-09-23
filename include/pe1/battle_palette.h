#ifndef PE1_BATTLE_PALETTE_H
#define PE1_BATTLE_PALETTE_H

#include "common.h"

/* Wide declarations keep palette bytes out of GCC -G8 small-data lowering. */
typedef struct BattlePaletteSymbol {
    u8 bytes[16];
} BattlePaletteSymbol;

extern BattlePaletteSymbol D_800B0134;
extern BattlePaletteSymbol D_800B0135;
extern BattlePaletteSymbol D_800B0136;
extern BattlePaletteSymbol D_800B013C;
extern BattlePaletteSymbol D_800B013D;
extern BattlePaletteSymbol D_800B013E;
extern BattlePaletteSymbol D_800B0144;
extern BattlePaletteSymbol D_800B0145;
extern BattlePaletteSymbol D_800B0146;
extern BattlePaletteSymbol D_800B014C;
extern BattlePaletteSymbol D_800B014D;
extern BattlePaletteSymbol D_800B014E;
extern BattlePaletteSymbol D_800B0158;
extern BattlePaletteSymbol D_800B0159;
extern BattlePaletteSymbol D_800B015A;
extern BattlePaletteSymbol D_800B0160;
extern BattlePaletteSymbol D_800B0161;
extern BattlePaletteSymbol D_800B0162;
extern BattlePaletteSymbol D_800B0168;
extern BattlePaletteSymbol D_800B0169;
extern BattlePaletteSymbol D_800B016A;
extern BattlePaletteSymbol D_800B0170;
extern BattlePaletteSymbol D_800B0171;
extern BattlePaletteSymbol D_800B0172;
extern BattlePaletteSymbol D_800B017C;
extern BattlePaletteSymbol D_800B017D;
extern BattlePaletteSymbol D_800B017E;
extern BattlePaletteSymbol D_800B0184;
extern BattlePaletteSymbol D_800B0185;
extern BattlePaletteSymbol D_800B0186;
extern BattlePaletteSymbol D_800B018C;
extern BattlePaletteSymbol D_800B018D;
extern BattlePaletteSymbol D_800B018E;
extern BattlePaletteSymbol D_800B0194;
extern BattlePaletteSymbol D_800B0195;
extern BattlePaletteSymbol D_800B0196;
extern BattlePaletteSymbol D_800B01A0;
extern BattlePaletteSymbol D_800B01A1;
extern BattlePaletteSymbol D_800B01A2;
extern BattlePaletteSymbol D_800B01A8;
extern BattlePaletteSymbol D_800B01A9;
extern BattlePaletteSymbol D_800B01AA;
extern BattlePaletteSymbol D_800B01B0;
extern BattlePaletteSymbol D_800B01B1;
extern BattlePaletteSymbol D_800B01B2;
extern BattlePaletteSymbol D_800B01B8;
extern BattlePaletteSymbol D_800B01B9;
extern BattlePaletteSymbol D_800B01BA;

extern BattlePaletteSymbol D_8009D254;
extern BattlePaletteSymbol D_800915E0;
extern BattlePaletteSymbol D_800B0CE6;
extern BattlePaletteSymbol D_8009D1A0_read __asm__("D_8009D1A0");
extern BattlePaletteSymbol D_8009D1A0_write __asm__("D_8009D1A0");
extern BattlePaletteSymbol D_8009D2E8_read __asm__("D_8009D2E8");
extern BattlePaletteSymbol D_8009D2E8_write __asm__("D_8009D2E8");
extern u8 D_8009D2EC;
extern u8 D_8009D2A0;

void Battle_FlushScriptSounds(void);
void BattleCmd_SyncActiveAmmo(void);
void Battle_SetupPlayerPalette(void);

#endif
