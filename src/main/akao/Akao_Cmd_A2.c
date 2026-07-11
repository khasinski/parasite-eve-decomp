extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;
extern int D_800BCD8C;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_A2(int arg0, int arg1, int arg2) {
    D_800BCD80 = 0xA2;
    D_800BCD84 = arg0 & 0x3FF;
    D_800BCD88 = arg1 & 0xFFFFFF;
    D_800BCD8C = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}
