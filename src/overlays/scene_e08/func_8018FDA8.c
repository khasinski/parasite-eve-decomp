#include "common.h"

void func_8018FDA8(void *unused, u8 *signal, u8 *state) {
    register u8 *byte asm("$4");
    register u8 *half asm("$8");
    int eight;
    unsigned i;
    int one;

    i = 0;
    one = 1;
    eight = 8;
    byte = state;
    half = state;
    /* Keep both stride bases independent through the loop. */
    asm volatile("" : "=r"(byte) : "0"(state));
    asm volatile("" : "=r"(half) : "0"(state));
    for (; i < 30; byte++, i++, half += 2) {
        u8 timer = byte[0x23A];
        int done;
        if (timer != 0) {
            timer--;
            byte[0x23A] = timer;
            if (timer == 0) {
                byte[0x21C] = one;
            }
        }

        if (byte[0x21C] != one) {
            continue;
        }

        *(u16 *)(half + 0x168) -= 0x10;
        byte[0x1E0]++;
        done = 0;

        if (byte[0x1A4] == 0x44) {
            *(u16 *)(half + 0xF0) += 0xC8;
            if (byte[0x1A4] == 0x44 && byte[0x1E0] == eight) {
                done = 1;
            }
        }
        if (byte[0x1A4] == 0x4C && byte[0x1E0] == eight) {
            done = 1;
        }
        if (byte[0x1A4] == 0x58 && byte[0x1E0] == 0x10) {
            done = 1;
        }
        if (done == one) {
            byte[0x21C] = 0;
            state[0x258]--;
            if (state[0x258] == 0) {
                signal[1] = 2;
            }
        }
    }
}
