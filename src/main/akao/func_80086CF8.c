extern int D_800BCD80;
extern int D_800BCD84;

int Akao_EnqueueStagedCommand(void);

void func_80086CF8(int arg0) {
    D_800BCD80 = 0xC8;
    D_800BCD84 = arg0;
    Akao_EnqueueStagedCommand();
}
