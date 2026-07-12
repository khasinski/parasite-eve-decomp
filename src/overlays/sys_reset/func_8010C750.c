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
    asm volatile("" ::: "memory");
    result = 0;
    asm volatile("" : "=r"(result) : "0"(result));
    (void)*D_8010DB30;
    *D_8010DB50 = 0x60000000;

    return result;
}
