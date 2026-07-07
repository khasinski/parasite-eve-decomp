/* CC1_FLAGS: -g3 */

int Math_Cmp64(unsigned int a_lo, unsigned int a_hi, unsigned int b_lo, unsigned int b_hi) {
    if (b_hi < a_hi) {
        return 1;
    }
    if (a_hi < b_hi) {
        return -1;
    }
    if (b_lo < a_lo) {
        return 1;
    }
    if (a_lo < b_lo) {
        return -1;
    }
    return 0;
}
