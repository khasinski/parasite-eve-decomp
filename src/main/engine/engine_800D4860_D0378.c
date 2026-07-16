typedef unsigned char u8;
typedef unsigned int u32;

#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define W32(base, off) (*(u32 *)((u8 *)(base) + (off)))

int func_800DFB78(u8 *arg0) {
    register u8 *state asm("$5");
    u8 *obj;
    register u32 flags asm("$4");
    register u32 masked asm("$2");
    register u8 *child asm("$2");
    u32 word;
    u32 byte;
    int phase;

    byte = arg0[0x19];
    obj = (u8 *)W32(arg0, 0x8);
    state = arg0 + 0xC;

    if ((byte & 4) && U8(obj, 0xE) < 2) {
        return 1;
    }

    if (state[0xD] & 2) {
        flags = W32(obj, 0x98);
        if (flags & 0x80000) {
            masked = flags & 0xFFF7FFFF;
            W32(obj, 0x98) = masked;
            return 1;
        }
    }

    if (state[0xD] & 1) {
        flags = W32(obj, 0x98);
        if (flags & 0x40000) {
            masked = flags & 0xFFFBFFFF;
            W32(obj, 0x98) = masked;
            return 1;
        }
    }

    child = (u8 *)W32(obj, 0x0);
    if (child != 0) {
        word = W32(child, 0x0);
        if (word & 0x1800) {
            return 1;
        }
        phase = (word >> 1) & 7;
        if (phase > 0) {
            return 1;
        }
    }

    return 0;
}
