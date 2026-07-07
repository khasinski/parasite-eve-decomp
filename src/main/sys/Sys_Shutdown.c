/* CC1_FLAGS: -g3 */

void Save_ProcessDataCallback(void);
void Akao_Cmd_D8(int arg0);
void Akao_Cmd_F0(void);
void Akao_Cmd_F1(void);
void Spu_Shutdown(void);
extern int Menu_IsEquipSlotActive(void);
int Menu_ResetEquipSlotState(void);

extern int g_SceneDispatchToken;
extern int D_800A77F4;
extern int g_GameState[];

void Sys_Shutdown(void) {
    int old;

    Save_ProcessDataCallback();
    Akao_Cmd_D8(0);
    Akao_Cmd_F0();
    Akao_Cmd_F1();
    Spu_Shutdown();

    if (Menu_IsEquipSlotActive() & 0xFF) {
        Menu_ResetEquipSlotState();
    }

    old = g_SceneDispatchToken;
    g_SceneDispatchToken = 0xA9400048;
    D_800A77F4 = old;
    g_GameState[0] |= 0x100;
}
