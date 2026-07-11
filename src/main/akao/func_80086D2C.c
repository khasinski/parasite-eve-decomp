extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD88;

int Akao_EnqueueStagedCommand(void);

void func_80086D2C(int arg0, int arg1) {
    D_800BCD80 = 0xC9;
    D_800BCD84 = arg0;
    D_800BCD88 = arg1;
    Akao_EnqueueStagedCommand();
}
