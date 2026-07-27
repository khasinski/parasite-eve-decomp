extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_A8(int arg0) {
    g_AkaoCmdOpcode = 0xA8;
    g_AkaoCmdArg0 = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}
