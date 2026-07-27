extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_D9(int arg0, int arg1) {
    g_AkaoCmdOpcode = 0xD9;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}
