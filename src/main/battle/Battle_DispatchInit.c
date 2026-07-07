/* CC1_FLAGS: -g3 */

extern int g_GameStateFlags;
extern int *g_PlayerEntity;
extern int g_ActiveActor;

void Battle_CopyPadStateToRecord(void);
void Battle_SetupEnemyAnims(void);
void Battle_SyncEnemyAttributes(void);

void Battle_DispatchInit(int arg0) {
    if (g_GameStateFlags & 2) {
        return;
    }

    g_ActiveActor = *g_PlayerEntity;

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    default:
        break;
    }
}
