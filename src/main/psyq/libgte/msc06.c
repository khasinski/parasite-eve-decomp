/*
 * PSY-Q LIBGTE MSC06: LoadAverage12, LoadAverage0, LoadAverageShort12,
 * LoadAverageShort0, LoadAverageByte and LoadAverageCol.
 */
#include "pe1/gte.h"

void LoadAverage12(void *first, void *second, int first_scale, int second_scale,
                   void * volatile output) {
    int lzcr;

    gte_ldir0(first_scale);
    gte_ldir123v(first);
    gte_gpf12();
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_ldir123v(second);
    gte_gpl12();
    gte_stir123_gpl(output);
}

void LoadAverage0(void *first, void *second, int first_scale, int second_scale,
                  void * volatile output) {
    int lzcr;

    gte_ldir0(first_scale);
    gte_ldir123v(first);
    gte_gpf0();
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_ldir123v(second);
    gte_gpl0();
    gte_stir123_gpl(output);
}

void LoadAverageShort12(void *first, void *second, int first_scale,
                        int second_scale, void * volatile output) {
    int lzcr;
    register int *out asm("$13");
    register int gte_x asm("$8");
    register int gte_y asm("$9");
    register int gte_z asm("$10");

    gte_x = ((int *)first)[0];
    gte_z = ((volatile int *)first)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    gte_ldir0(first_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_mtc2_11(gte_z);
    gte_gpf12();
    gte_x = ((int *)second)[0];
    gte_z = ((volatile int *)second)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_mtc2_11(gte_z);
    gte_gpl12();
    gte_mfc2_9(gte_x);
    gte_mfc2_10(gte_y);
    gte_x &= 0xFFFF;
    gte_y = (unsigned int)gte_y << 16;
    gte_x |= gte_y;
    out = output;
    gte_mfc2_11(gte_z);
    ((volatile int *)out)[0] = gte_x;
    ((volatile int *)out)[1] = gte_z;
}

void LoadAverageShort0(void *first, void *second, int first_scale,
                       int second_scale, void * volatile output) {
    int lzcr;
    register int *out asm("$13");
    register int gte_x asm("$8");
    register int gte_y asm("$9");
    register int gte_z asm("$10");

    gte_x = ((int *)first)[0];
    gte_z = ((volatile int *)first)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    gte_ldir0(first_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_mtc2_11(gte_z);
    gte_gpf0();
    gte_x = ((int *)second)[0];
    gte_z = ((volatile int *)second)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_mtc2_11(gte_z);
    gte_gpl0();
    gte_mfc2_9(gte_x);
    gte_mfc2_10(gte_y);
    gte_x &= 0xFFFF;
    gte_y = (unsigned int)gte_y << 16;
    gte_x |= gte_y;
    out = output;
    gte_mfc2_11(gte_z);
    ((volatile int *)out)[0] = gte_x;
    ((volatile int *)out)[1] = gte_z;
}

void LoadAverageByte(void *first, void *second, int first_scale,
                     int second_scale, void * volatile output) {
    int lzcr;
    register unsigned char *out asm("$13");
    register int gte_x asm("$8");
    register int gte_y asm("$9");
    register int gte_shift asm("$11");

    gte_load_byte2(first);
    gte_ldir0(first_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_gpf0();
    gte_load_byte2(second);
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_set_shift12();
    gte_gpl0_now();
    out = output;
    gte_getmac12_staged(out);
    gte_x >>= gte_shift;
    gte_y >>= gte_shift;
    out[0] = gte_x;
    out[1] = gte_y;
    asm volatile("" : : : "memory");
}

void LoadAverageCol(void *first, void *second, int first_scale,
                    int second_scale, void * volatile output) {
    int lzcr;
    register unsigned char *out asm("$13");
    register int gte_x asm("$8");
    register int gte_y asm("$9");
    register int gte_z asm("$10");
    register int gte_shift asm("$11");

    gte_load_byte3(first);
    gte_ldir0(first_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_mtc2_11(gte_z);
    gte_gpf0();
    gte_load_byte3(second);
    gte_getlzcr_now(lzcr);
    gte_ldir0(second_scale);
    gte_mtc2_9(gte_x);
    gte_mtc2_10(gte_y);
    gte_mtc2_11(gte_z);
    gte_set_shift12();
    gte_gpl0_now();
    out = output;
    gte_getmac123_staged(out);
    gte_x >>= gte_shift;
    gte_y >>= gte_shift;
    gte_z >>= gte_shift;
    out[0] = gte_x;
    out[1] = gte_y;
    out[2] = gte_z;
    asm volatile("" : : : "memory");
}
