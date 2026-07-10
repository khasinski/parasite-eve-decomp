#define LOAD_SIN_TABLE(offset, tableOffset)                                      \
    ({                                                                           \
        register int value asm("$2") = (offset);                                 \
        asm volatile(".set noat\n"                                               \
                     "lui $at,0x8009\n"                                         \
                     "addu $at,$at,%0\n"                                        \
                     "lh %0," tableOffset "($at)\n"                             \
                     ".set at"                                                   \
                     : "=r"(value)                                               \
                     : "0"(value));                                              \
        value;                                                                   \
    })

int rcos(int angle) {
    int value;

    if (angle < 0) {
        angle = -angle;
    }

    angle &= 0xFFF;
    if (angle < 0x801) {
        if (angle < 0x401) {
            return LOAD_SIN_TABLE((0x400 - angle) << 1, "0x589C");
        }

        value = LOAD_SIN_TABLE(angle << 1, "0x509C");
        return -value;
    }

    if (angle < 0xC01) {
        value = LOAD_SIN_TABLE((0xC00 - angle) << 1, "0x589C");
        asm volatile("nop");
        return -value;
    }

    return LOAD_SIN_TABLE(angle << 1, "0x409C");
}
