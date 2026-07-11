extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;
extern int D_800BCD90;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2) {
    D_800BCD80 = 0xC1;
    D_800BCD84 = arg1;
    D_800BCD88 = arg2 & 0x7F;
    D_800BCD90 = arg0;
    Akao_EnqueueStagedCommand();
}
