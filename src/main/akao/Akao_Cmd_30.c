extern int D_800BCD80;
extern int D_800BCD84;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_30(int arg0) {
    D_800BCD80 = 0x30;
    D_800BCD84 = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}
