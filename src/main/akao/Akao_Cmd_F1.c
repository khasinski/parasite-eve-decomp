extern int D_800BCD80;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_F1(void) {
    D_800BCD80 = 0xF1;
    Akao_EnqueueStagedCommand();
}
