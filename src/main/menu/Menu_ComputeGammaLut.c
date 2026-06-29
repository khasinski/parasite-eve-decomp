/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

int g_MenuSaveBgFadeLutLen;
extern u8 D_800A1878[];

void Menu_ComputeGammaLut(int arg0, int arg1) {
    u8 *base;
    u8 *ptr;
    int scale;
    int limit;
    u8 *iter_end;
    u8 *check_end;

    base = D_800A1878;
    base[0] = 0;
    ptr = base;
    limit = 0x100;
    scale = limit - arg0;
    check_end = ptr + 0xF;
    arg0 <<= 8;
    if (ptr < check_end) {
        iter_end = check_end;
        do {
            if (*ptr >= arg1) {
                goto out;
            }
            ptr[1] = (arg0 + (scale * *ptr)) >> 8;
            ptr++;
        } while (ptr < iter_end);
    }

out:
    base = D_800A1878;
    limit = (ptr - base) + 1;
    g_MenuSaveBgFadeLutLen = limit;
}
