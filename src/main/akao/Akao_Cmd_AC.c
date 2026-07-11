extern int D_800BCD80;
extern int D_800BCD84;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_AC(int arg0) {
    D_800BCD80 = 0xAC;
    D_800BCD84 = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}
