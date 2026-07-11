extern int D_800BCD80;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_40(void) {
    D_800BCD80 = 0x40;
    Akao_EnqueueStagedCommand();
}
