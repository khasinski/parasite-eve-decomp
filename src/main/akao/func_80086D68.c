extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;
extern int D_800BCD8C;

int Akao_EnqueueStagedCommand(void);

void func_80086D68(int arg0, int arg1, int arg2) {
    D_800BCD80 = 0xCA;
    D_800BCD84 = arg0;
    D_800BCD88 = arg1;
    D_800BCD8C = arg2;
    Akao_EnqueueStagedCommand();
}
