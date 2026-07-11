extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_A9(int arg0, int arg1) {
    D_800BCD80 = 0xA9;
    D_800BCD84 = arg0 & 0xFF;
    D_800BCD88 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}
