extern int D_800BCD80;
extern int D_800BCD84;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_D4(int arg0) {
    D_800BCD80 = 0xD4;
    D_800BCD84 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}
