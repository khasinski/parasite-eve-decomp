/* MASPSX_FLAGS: --store-return-delay */
typedef unsigned char u8;

int func_800CD5A4(void *arg0, u8 *state) {
    register int ret asm("$2") = 2;
    state[1] = ret;
    return ret;
}
