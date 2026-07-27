extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;
extern int g_AkaoCmdArg2;
extern int g_AkaoCmdArg3;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmdOpcode = 0xC2;
    g_AkaoCmdArg0 = arg1;
    g_AkaoCmdArg1 = arg2 & 0x7F;
    g_AkaoCmdArg2 = arg3 & 0x7F;
    g_AkaoCmdArg3 = arg0;
    Akao_EnqueueStagedCommand();
}
