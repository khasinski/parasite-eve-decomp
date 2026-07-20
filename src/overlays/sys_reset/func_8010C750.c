typedef int s32;
typedef unsigned int u32;
typedef volatile unsigned int vu32;

s32 func_80071A74(void *arg0, s32 arg1);

extern char D_8010BD38[];
extern vu32 *D_8010DB50;
extern vu32 *D_8010DB24;
extern vu32 *D_8010DB30;

s32 func_8010C750(s32 arg0) {
    func_80071A74(D_8010BD38, arg0);
    *D_8010DB50 = 0x80000000;
    *D_8010DB24 = 0;
    *D_8010DB30 = 0;
    *D_8010DB30;
    *D_8010DB50 = 0x60000000;
    return 0;
}
