typedef unsigned char u8;
typedef unsigned int u32;

void func_800C3134(u8 *table, u32 step, u8 *out) {
    u32 sum = 0;
    u32 one = 0x1000;
    u32 last = 0xFF;
    u8 *entry = table;

    while (1) {
        u32 duration = entry[3];

        sum += duration;
        if (step < sum) {
            u32 weight = ((sum - step) << 12) / duration;
            u32 inv = one - weight;

            out[0] = ((inv * entry[4]) + (weight * entry[0])) >> 12;
            out[1] = ((inv * entry[5]) + (weight * entry[1])) >> 12;
            out[2] = ((inv * entry[6]) + (weight * entry[2])) >> 12;
            return;
        }

        if (entry[7] == last) {
            out[0] = entry[4];
            out[1] = entry[5];
            out[2] = entry[6];
            return;
        }

        entry += 4;
    }
}
