extern void *D_80192260;

int func_801831B0(char *obj, int mode, int event, int arg3, int arg4, int arg5) {
    register int arg4_reg asm("$8");
    register int arg5_reg asm("$3");
    register int cmp asm("$2");

    arg4_reg = arg4;
    arg5_reg = arg5;
    cmp = 0xA;

    if (event == cmp) {
        goto case_0A;
    }


    if ((unsigned int)event >= 0xB) {
        goto high_events;
    }
    cmp = 4;
    if (event == cmp) {
        goto case_04;
    }
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80192228\n\t"
        "nop\n\t"
        ".set\treorder");

high_events:
    cmp = 0x19;
    if (event == cmp) {
        goto case_19;
    }
    cmp = 0x1C;
    if (event == cmp) {
        goto case_1C;
    }
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80192228\n\t"
        "nop\n\t"
        ".set\treorder");

case_19:
    cmp = 1;
    if (mode != cmp) {
        goto done;
    }
    *(int *)(obj + 0x10) = arg3;
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80192228\n\t"
        "sw\t%1, 0(%0)\n\t"
        ".set\treorder"
        :
        : "r"(arg3), "r"(mode));

case_04:
    *(int *)(obj + 0x3C) = arg3;
    *(int *)(obj + 0x40) = arg4_reg;
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80192228\n\t"
        "sh\t%1, 0x44(%0)\n\t"
        ".set\treorder"
        :
        : "r"(obj), "r"(arg5_reg));

case_1C:
    *(short *)(obj + 0x48) = arg3;
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80192228\n\t"
        "sh\t%1, 0x46(%0)\n\t"
        ".set\treorder"
        :
        : "r"(obj), "r"(arg4_reg));

case_0A:
    obj[0x16] = arg3;
    *(void **)(obj + 0xC) = &D_80192260;

done:
    return 0;
}
