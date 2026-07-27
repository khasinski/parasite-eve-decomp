extern int g_AkaoCmdOpcode;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_F1(void) {
    g_AkaoCmdOpcode = 0xF1;
    Akao_EnqueueStagedCommand();
}
