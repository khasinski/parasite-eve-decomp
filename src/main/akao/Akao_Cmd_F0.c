extern int g_AkaoCmdOpcode;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_F0(void) {
    g_AkaoCmdOpcode = 0xF0;
    Akao_EnqueueStagedCommand();
}
