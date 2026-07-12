extern int D_800BCD80;
extern int D_800BCD84;
extern int D_800BCD90;

int Akao_EnqueueStagedCommand(void);

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    register int *opcode asm("$17");
    register int saved_arg asm("$16");
    register int next_opcode asm("$3");
    int ret;

    opcode = &D_800BCD80;
    *opcode = 0x19;
    D_800BCD84 = arg0;
    saved_arg = arg1;
    ret = Akao_EnqueueStagedCommand();

    next_opcode = 0xC0;
    asm volatile("" : : "r"(next_opcode));
    *opcode = next_opcode;
    saved_arg &= 0x7F;
    D_800BCD84 = saved_arg;
    D_800BCD90 = 0;
    saved_arg = ret;
    Akao_EnqueueStagedCommand();

    return saved_arg;
}
