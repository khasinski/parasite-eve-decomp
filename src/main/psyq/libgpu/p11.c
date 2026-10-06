/* Psy-Q LIBGPU P11.OBJ: SetShadeTex. */

void SetShadeTex(unsigned char *packet, int enabled) {
    if (enabled != 0) {
        packet[7] |= 1;
    } else {
        packet[7] &= 0xFE;
    }
}

unsigned int SetShadeTex_alignment[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
