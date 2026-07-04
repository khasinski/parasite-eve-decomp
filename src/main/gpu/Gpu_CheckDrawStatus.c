/* CC1_PROFILE: build0001 */

typedef unsigned short u16;

extern signed char g_DrawEnabled;
extern signed char D_800B0DBB;
extern u16 g_SeqElapsed;

int Gpu_CheckDrawStatus(void) {
    int enabled = g_DrawEnabled;
    int enabled2;
    register int value asm("$2");
    int result;
    register int flag asm("$3");

    asm volatile("addiu $sp,$sp,-8");
    if (enabled != 0) {
        asm volatile("" : "=r"(enabled2) : "0"(enabled));
        value = -1;
        if (enabled2 != 0) {
            value = g_SeqElapsed;
            value <<= 16;
        } else {
            asm("sll %0,%1,16" : "=r"(value) : "0"(value));
        }
        if (value > 0) {
            flag = D_800B0DBB;
            if (flag != 0) {
                result = 2;
                goto done;
            }
            result = 1;
            goto done;
        }
    }

    result = 0;

done:
    asm volatile("addiu $sp,$sp,8");
    return result;
}
