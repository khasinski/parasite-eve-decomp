extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;
extern int D_800BCD8C;
extern int D_800BCD90;

int Spu_ValidateSampleHeader(void);
int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3) {
    if (Spu_ValidateSampleHeader() != 0) {
        return;
    }

    D_800BCD80 = 0x24;
    D_800BCD84 = arg0 + 4;
    D_800BCD88 = arg1 & 0xFFFFFF;
    D_800BCD8C = arg2 & 0xFF;
    D_800BCD90 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}
