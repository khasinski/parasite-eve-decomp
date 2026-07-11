extern int D_800BCD80;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_F0(void) {
    D_800BCD80 = 0xF0;
    Akao_EnqueueStagedCommand();
}
