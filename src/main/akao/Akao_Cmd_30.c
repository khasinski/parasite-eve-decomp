extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_30(int arg0) {
    g_AkaoCmdOpcode = 0x30;
    g_AkaoCmdArg0 = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}
