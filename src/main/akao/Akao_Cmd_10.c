extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_10(int arg0) {
    g_AkaoCmdOpcode = 0x10;
    g_AkaoCmdArg0 = arg0;
    Akao_EnqueueStagedCommand();
}
