void Akao_Cmd_F0(void);

extern signed char D_800B0DB2;
extern signed char D_800B0DB3;
extern signed char D_800B0DB4;
extern signed char D_800B0DB5;
extern signed char D_800B0DB6;
extern signed char D_800B0DB7;
extern unsigned int g_GameState;

void Akao_ResetPositionalState(void) {
    D_800B0DB5 = -1;
    D_800B0DB4 = -1;
    D_800B0DB7 = -1;
    D_800B0DB6 = -1;
    D_800B0DB3 = -1;
    D_800B0DB2 = -1;
    g_GameState &= -0xF1;
    Akao_Cmd_F0();
}
