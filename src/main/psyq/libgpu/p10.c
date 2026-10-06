/* Psy-Q LIBGPU P10.OBJ: SetSemiTrans. */

void SetSemiTrans(unsigned char *packet, int enabled) {
    if (enabled != 0) {
        packet[7] |= 2;
    } else {
        packet[7] &= 0xFD;
    }
}

unsigned int SetSemiTrans_alignment[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
