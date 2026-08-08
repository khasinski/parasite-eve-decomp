extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg3;

int Akao_EnqueueStagedCommand(void);

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    register int *opcode asm("$17");
    register int saved_arg asm("$16");
    int next_opcode;
    int ret;

    opcode = &g_AkaoCmdOpcode;
    *opcode = 0x19;
    g_AkaoCmdArg0 = arg0;
    saved_arg = arg1;
    ret = Akao_EnqueueStagedCommand();

    next_opcode = 0xC0;
    asm volatile("" : : "r"(next_opcode));
    *opcode = next_opcode;
    saved_arg &= 0x7F;
    g_AkaoCmdArg0 = saved_arg;
    g_AkaoCmdArg3 = 0;
    saved_arg = ret;
    Akao_EnqueueStagedCommand();

    return saved_arg;
}
