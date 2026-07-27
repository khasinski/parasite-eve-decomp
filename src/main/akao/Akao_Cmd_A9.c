extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xA9;
    g_AkaoCmdArg0 = arg0 & 0xFF;
    g_AkaoCmdArg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}
