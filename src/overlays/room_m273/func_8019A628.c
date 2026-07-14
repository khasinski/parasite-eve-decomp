typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern void **D_800F33E0;
extern s32 D_800E27EC;
extern u16 D_8019AEFC;
extern u16 D_8019AEFE;
extern u16 D_8019AF00;
extern u8 D_8019AF6B;

void func_8019A720(void);
s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, void *arg3);
u16 *func_800CE610(void *arg0);

s32 func_8019A628(s32 arg0) {
    register u16 *actor asm("$3");
    register s32 ret asm("$2");

    ret = 1;
    if (arg0 == ret) {
        goto case1;
    }

    if (arg0 >= 2) {
        goto ret_zero;
    }
    if (arg0 != 0) {
        ret = 0;
        goto done;
    }

    ret = func_800CE560(D_800F33E0[2], 8, 5, func_8019A720);
    goto done;

case1:
    if (D_8019AF6B != 0) {
        ret = 2;
        goto done;
    }
    if ((D_800E27EC & 3) != 0) {
        ret = 0;
        goto done;
    }

    actor = func_800CE610(D_800F33E0[2]);
    asm volatile("" : "=r"(actor) : "0"(actor));
    if (actor != 0) {
        actor[0] = D_8019AEFC;
        actor[1] = D_8019AEFE;
        asm volatile("" ::: "memory");
        actor[3] = 0;
        actor[2] = D_8019AF00;
    }

ret_zero:
    ret = 0;
done:
    return ret;
}
