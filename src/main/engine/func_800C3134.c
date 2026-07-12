/* MASPSX_FLAGS: --expand-div */
typedef unsigned char u8;
typedef unsigned int u32;

void func_800C3134(u8 *table, u32 step, u8 *out) {
    u32 sum = 0;
    register u32 one asm("$10") = 0x1000;
    register u32 last asm("$9") = 0xFF;
    register u8 *entry asm("$7") = table;

    while (1) {
        u32 duration = entry[3];
        u8 *next = entry + 4;

        sum += duration;
        if (step < sum) {
            u32 weight = ((sum - step) << 12) / duration;
            register u32 inv asm("$4") = one - weight;
            u32 out0;
            u32 out1;
            u32 out2;

            out0 = ((inv * entry[4]) + (weight * entry[0])) >> 12;
            out1 = ((inv * entry[5]) + (weight * entry[1])) >> 12;
            out2 = ((inv * entry[6]) + (weight * entry[2])) >> 12;
            out[0] = out0;
            out[1] = out1;
            out[2] = out2;
            return;
        }

        if (entry[7] == last) {
            out[0] = entry[4];
            out[1] = entry[5];
            out[2] = entry[6];
            return;
        }

        entry = next;
    }
}
