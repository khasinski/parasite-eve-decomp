typedef signed short s16;
typedef unsigned char u8;

void func_8018FA64(void *arg0, u8 *state) {
    if (*(s16 *)(state + 2) == 0x3A) {
        state[1] = 2;
    }
}
