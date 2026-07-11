typedef unsigned int u32;

extern u32 D_8009D26C;
extern u32 D_8009D1F4;
extern u32 D_8009D1E4;
extern u32 D_800A7770[];

static int Task_CountLeadingZeroes(u32 value) {
    int count;

    count = 0;
    while ((value & 0x80000000) == 0 && count < 32) {
        value <<= 1;
        count++;
    }
    return count;
}

static int Task_MaskFullySet(u32 bits, u32 mask) {
    return (bits & mask) == mask;
}

int Task_DispatchCmd(int **args) {
    int type;
    int *mask_ptr;
    int *out;
    u32 mask;
    int leading;

    type = *args[0];
    mask_ptr = args[1];
    out = args[2];

    if (type == 0) {
        *out = Task_MaskFullySet(D_8009D26C, *mask_ptr);
    } else if (type == 1) {
        *out = Task_MaskFullySet(D_8009D1F4, *mask_ptr);
    } else if (type == 2) {
        *out = Task_MaskFullySet(D_8009D1E4, *mask_ptr);
    } else if (type == 3) {
        mask = *mask_ptr;
        if (!Task_MaskFullySet(D_8009D26C, mask)) {
            *out = 0;
        } else {
            if (mask == 0x80000000) {
                leading = 0;
                *out = D_800A7770[31];
            } else {
                leading = Task_CountLeadingZeroes(mask);
                *mask_ptr = leading;
                *out = D_800A7770[31 - leading];
            }
        }
    }

    return 1;
}
