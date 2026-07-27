extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;
extern int g_AkaoCmdArg2;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_A0(int arg0, int arg1, int arg2) {
    g_AkaoCmdOpcode = 0xA0;
    g_AkaoCmdArg0 = arg0 & 0xFFFF;
    g_AkaoCmdArg1 = arg1 & 0xFFFFFF;
    g_AkaoCmdArg2 = arg2 & 0x7F;
    Akao_EnqueueStagedCommand();
}
