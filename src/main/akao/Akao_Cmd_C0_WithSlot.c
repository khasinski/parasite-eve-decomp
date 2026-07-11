extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD90;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    D_800BCD80 = 0xC0;
    D_800BCD84 = arg1 & 0x7F;
    D_800BCD90 = arg0;
    Akao_EnqueueStagedCommand();
}
