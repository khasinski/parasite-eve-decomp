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

int sin_1(int angle);

int rsin(int angle) {
    if (angle < 0) {
        return -sin_1((-angle) & 0xFFF);
    }

    return sin_1(angle & 0xFFF);
}

int sin_1(int angle) {
    int value;

    if (angle < 0x801) {
        if (angle < 0x401) {
            return LOAD_SIN_TABLE(angle << 1, "0x589C");
        }

        return LOAD_SIN_TABLE((0x800 - angle) << 1, "0x589C");
    }

    if (angle < 0xC01) {
        value = LOAD_SIN_TABLE(angle << 1, "0x489C");
        asm volatile("nop");
        return -value;
    }

    value = LOAD_SIN_TABLE((0x1000 - angle) << 1, "0x589C");
    return -value;
}
