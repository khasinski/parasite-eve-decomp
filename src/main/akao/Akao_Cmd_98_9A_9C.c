extern int D_800BCD80;

int Akao_EnqueueStagedCommand(void);

void Akao_Cmd_98_9A_9C(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9A;
        break;
    case 2:
        opcode = 0x9C;
        break;
    default:
        opcode = 0x98;
        break;
    }

    D_800BCD80 = opcode;
    Akao_EnqueueStagedCommand();
}
