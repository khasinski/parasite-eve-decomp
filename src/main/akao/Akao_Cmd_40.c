extern int g_AkaoCmdOpcode;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_40(void) {
    g_AkaoCmdOpcode = 0x40;
    Akao_EnqueueStagedCommand();
}
