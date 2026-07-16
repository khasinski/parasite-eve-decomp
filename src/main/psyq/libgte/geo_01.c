#define LOAD_SIN_TABLE_409C(offset)                                              \
    ({                                                                           \
        register int value asm("$2") = (offset);                                 \
        asm volatile(".set noat\n"                                               \
                     "lui $at,0x8009\n"                                         \
                     "addu $at,$at,%0\n"                                        \
                     "lh %0,0x409C($at)\n"                                      \
                     ".set at"                                                   \
                     : "=r"(value)                                               \
                     : "0"(value));                                              \
        value;                                                                   \
    })

#define LOAD_SIN_TABLE_509C(offset)                                              \
    ({                                                                           \
        register int value asm("$2") = (offset);                                 \
        asm volatile(".set noat\n"                                               \
                     "lui $at,0x8009\n"                                         \
                     "addu $at,$at,%0\n"                                        \
                     "lh %0,0x509C($at)\n"                                      \
                     ".set at"                                                   \
                     : "=r"(value)                                               \
                     : "0"(value));                                              \
        value;                                                                   \
    })

#define LOAD_SIN_TABLE_589C(offset)                                              \
    ({                                                                           \
        register int value asm("$2") = (offset);                                 \
        asm volatile(".set noat\n"                                               \
                     "lui $at,0x8009\n"                                         \
                     "addu $at,$at,%0\n"                                        \
                     "lh %0,0x589C($at)\n"                                      \
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
            return LOAD_SIN_TABLE_589C((0x400 - angle) << 1);
        }

        value = LOAD_SIN_TABLE_509C(angle << 1);
        return -value;
    }

    if (angle < 0xC01) {
        value = LOAD_SIN_TABLE_589C((0xC00 - angle) << 1);
        asm volatile("nop");
        return -value;
    }

    return LOAD_SIN_TABLE_409C(angle << 1);
}
