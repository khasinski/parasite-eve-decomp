/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

unsigned char g_BattleActionQueueCount;
extern int g_GameStateFlags[];
extern char *g_ActiveActor[];

int Battle_IsActive(void) {
    if (g_BattleActionQueueCount != 0) {
        goto no;
    }
    if ((g_GameStateFlags[0] & 2) == 0) {
        goto yes;
    }
    if ((*(int *)(g_ActiveActor[0] + 0x4C) & 0x10000) != 0) {
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}

int Pad_IsMenuConfirmAvailable(void) {
    if (g_BattleActionQueueCount != 0) {
        goto no;
    }
    if ((g_GameStateFlags[0] & 2) == 0) {
        goto yes;
    }
    if ((*(int *)(g_ActiveActor[0] + 0x4C) & 0x10000) != 0) {
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}
