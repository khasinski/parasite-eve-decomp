/* MASPSX_FLAGS: --store-return-delay --stack-return-delay --la-call-delay */

typedef unsigned char u_char;
typedef unsigned int u32;

extern char D_80011D74[];
extern char D_80011D94[];
extern char D_80011DB0[];
extern char D_80011DD8[];
extern char D_80011DF8[];
extern char D_80011E0C[];
extern char D_80011E3C[];
extern char D_80011E44[];
extern char D_80011E4C[];
extern u32 D_8009B574[];
extern u32 D_800A36A0;
extern u32 D_800A36A4;
extern u32 D_800A36A8;

int DsRead_IsBusy(void);
int printf(char *fmt, ...);

void DS_status(void) {
    register u32 *state asm("$16");
    register int b0 asm("$5");
    register int b1 asm("$6");
    register int b2 asm("$7");
    register int b3 asm("$2");
    register int b4 asm("$3");
    char *busy;

    printf(D_80011D74);

    state = D_8009B574;
    asm volatile("" : "=r"(state) : "0"(state));
    printf(D_80011D94, state[0], state[1], state[2]);
    b0 = ((u_char *)state)[-0x1C];
    b1 = ((u_char *)state)[-0x1B];
    b2 = ((u_char *)state)[-0x1A];
    b3 = ((u_char *)state)[-0x19];
    b4 = ((u_char *)state)[-0x18];
    printf(D_80011DB0, b0, b1, b2, b3, b4);
    printf(D_80011DD8, state[9]);
    printf(D_80011DF8, state[8]);
    printf(D_80011E0C, D_800A36A0, D_800A36A4, D_800A36A8);
    asm volatile("" ::: "memory");

    if (DsRead_IsBusy() != 0) {
        busy = D_80011E3C;
    } else {
        busy = D_80011E44;
    }
    printf(D_80011E4C, busy);
}
