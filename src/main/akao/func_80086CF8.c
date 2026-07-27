extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;

int Akao_EnqueueStagedCommand(void);

void func_80086CF8(int arg0) {
    g_AkaoCmdOpcode = 0xC8;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
