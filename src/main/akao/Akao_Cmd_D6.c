extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;
extern int g_AkaoCmdArg2;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_D6(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xD6;
    g_AkaoCmdArg0 = arg0;
    g_AkaoCmdArg1 = arg1 & 0xFF;
    g_AkaoCmdArg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}
