extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_AA(int arg0) {
    g_AkaoCmdOpcode = 0xAA;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}
