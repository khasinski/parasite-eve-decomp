typedef unsigned int u32;

int Math_Cmp64Pair(u32 lhs_lo, u32 lhs_hi, u32 rhs_lo, u32 rhs_hi) {
    if (rhs_hi < lhs_hi) {
        return 1;
    }

    if (lhs_hi < rhs_hi) {
        return -1;
    }

    if (rhs_lo < lhs_lo) {
        return 1;
    }

    if (lhs_lo < rhs_lo) {
        return -1;
    }

    return 0;
}
