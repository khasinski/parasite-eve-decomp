extern int D_800BCD80;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_99_9B_9D(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9B;
        break;
    case 2:
        opcode = 0x9D;
        break;
    default:
        opcode = 0x99;
        break;
    }

    D_800BCD80 = opcode;
    Akao_EnqueueStagedCommand();
}
