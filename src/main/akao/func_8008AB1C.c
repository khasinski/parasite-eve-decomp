typedef unsigned short u16;
typedef unsigned int u32;

extern u16 *D_8009D240;
extern char *D_8009D260;

void func_8008AB1C(int *out0, int *out1, int sample_id) {
    u32 index;
    u16 value;
    int result;

    index = sample_id;
    index &= 0x3FF;
    index <<= 1;

    value = D_8009D240[index];
    if (value != 0xFFFF) {
        result = (int)(D_8009D260 + value);
    } else {
        result = 0;
    }
    *out0 = result;

    index++;
    value = D_8009D240[index];
    if (value != 0xFFFF) {
        result = (int)(D_8009D260 + value);
    } else {
        result = 0;
    }
    *out1 = result;
}
