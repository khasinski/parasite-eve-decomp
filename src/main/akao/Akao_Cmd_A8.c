extern int D_800BCD80;
extern int D_800BCD84;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_A8(int arg0) {
    D_800BCD80 = 0xA8;
    D_800BCD84 = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}
