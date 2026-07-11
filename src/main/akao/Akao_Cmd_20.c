extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;
extern int D_800BCD8C;
extern int D_800BCD90;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_20(int arg0, int arg1, int arg2, int arg3) {
    D_800BCD80 = 0x20;
    D_800BCD84 = arg0 & 0x3FF;
    D_800BCD88 = arg1 & 0xFFFFFF;
    D_800BCD8C = arg2 & 0xFF;
    D_800BCD90 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}
