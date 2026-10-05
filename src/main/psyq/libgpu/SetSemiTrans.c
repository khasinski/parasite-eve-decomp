void Gpu_SetDither(unsigned char *packet, int enabled) {
    if (enabled != 0) {
        packet[7] |= 2;
    } else {
        packet[7] &= 0xFD;
    }
}

unsigned int Gpu_SetDither_alignment[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};

void Gpu_SetDrawEnable(unsigned char *packet, int enabled) {
    if (enabled != 0) {
        packet[7] |= 1;
    } else {
        packet[7] &= 0xFE;
    }
}

unsigned int Gpu_SetDrawEnable_alignment[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
