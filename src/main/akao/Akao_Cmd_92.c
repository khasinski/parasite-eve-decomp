extern int D_800BCD80;
extern int D_800BCD84;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_92(int arg0) {
    D_800BCD80 = 0x92;
    D_800BCD84 = arg0;
    Akao_EnqueueStagedCommand();
}
