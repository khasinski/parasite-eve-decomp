/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct BattleCmdEntry BattleCmdEntry;

extern BattleCmdEntry *D_8009D014;
extern BattleCmdEntry D_800A1AA0[];

void BattleCmd_UndoPending(void);

void func_80051258(void) {
    if (D_800A1AA0 < D_8009D014) {
        BattleCmdEntry *base = D_800A1AA0;

        do {
            BattleCmd_UndoPending();
        } while (base < D_8009D014);
    }
}
