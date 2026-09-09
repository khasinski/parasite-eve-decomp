/* CC1_FLAGS: -g3 */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

typedef unsigned char u8;
typedef unsigned int u32;

extern u32 D_8009AF2C[];
extern u32 D_8009AFB4;
extern u8 D_8009AFC4;

int CD_cw(int arg0, void *arg1, int arg2, int arg3);

int func_8007A4D0(int cmd, void *param, int extra) {
    register int tries;
    register void *param_reg;
    register int extra_reg;
    register int cmd_reg;
    register int cmd_byte;
    register u32 saved;
    register u32 *slot;
    register int ret;
    register int minus_one;
    int one;

    param_reg = param;
    extra_reg = extra;
    cmd_reg = cmd;
    tries = 3;
    cmd_byte = cmd_reg & 0xFF;
    saved = D_8009AFB4;
    slot = &D_8009AF2C[cmd_byte];
    ret = 0;
    asm volatile("" : "+r"(ret));
    minus_one = -1;
    asm volatile("" : "+r"(minus_one));

loop:
    D_8009AFB4 = 0;
    one = 1;
    if (cmd_byte != one && (D_8009AFC4 & 0x10) != 0) {
        CD_cw(1, 0, 0, 0);
    }
    if (param_reg != 0 && slot[0] != 0) {
        if (CD_cw(2, param_reg, extra_reg, 0) != 0) {
            goto retry;
        }
    }
    D_8009AFB4 = saved;
    if (CD_cw(cmd_reg & 0xFF, param_reg, extra_reg, 0) != 0) {
        goto retry;
    }
    return ret + 1;

retry:
    tries--;
    if (tries != minus_one) {
        goto loop;
    }
    D_8009AFB4 = saved;
    ret = -1;
    asm volatile("" : "+r"(ret));
    return ret + 1;
}
