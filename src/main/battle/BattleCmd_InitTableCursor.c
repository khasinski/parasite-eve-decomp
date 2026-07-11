/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct BattleCmdEntry BattleCmdEntry;

extern BattleCmdEntry D_800A1AA0[];
extern BattleCmdEntry *D_8009D014;

void BattleCmd_InitTableCursor(void) {
    D_8009D014 = D_800A1AA0;
}
