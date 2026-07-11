extern volatile int *D_8010DB24;
extern volatile int *D_8010DB30;
extern volatile int *D_8010DB50;
extern char D_8010BD38[];

void func_80071A74(char *message, int value);

int func_8010C750(int value) {
    register int result asm("$2");

    func_80071A74(D_8010BD38, value);

    *D_8010DB50 = 0x80000000;
    *D_8010DB24 = 0;
    *D_8010DB30 = 0;
    __asm__ volatile(
        "addu       %0, $0, $0\n"
        "lui        $3, %%hi(D_8010DB30)\n"
        "lw         $3, %%lo(D_8010DB30)($3)\n"
        "lui        $4, %%hi(D_8010DB50)\n"
        "lw         $4, %%lo(D_8010DB50)($4)\n"
        "lw         $3, 0($3)\n"
        "lui        $3, 0x6000\n"
        "sw         $3, 0($4)\n"
        : "=r"(result)
        :
        : "$3", "$4", "memory");

    return result;
}
