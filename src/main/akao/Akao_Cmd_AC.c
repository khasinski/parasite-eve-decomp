extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmdOpcode = 0xAC;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}
