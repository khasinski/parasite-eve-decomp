void Spu_VoiceMaskCompose(unsigned char *obj, int *mask_out, int mask, int mask_keep) {
    int bit;
    int idx;

    bit = 1;
    do {
        if ((mask & bit) != 0) {
            idx = *(int *) (obj + 0xF0);
            if ((unsigned int) idx < 0x18) {
                *mask_out |= 1 << idx;
            }
        }
        mask &= ~bit;
        obj = (unsigned char *) ((int *) obj + 0x11C / 4);
        bit <<= 1;
    } while (mask != 0);

    *mask_out &= mask_keep;
}
