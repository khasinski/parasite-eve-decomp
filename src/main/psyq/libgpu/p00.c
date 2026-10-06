/* Psy-Q LIBGPU P00.OBJ: GetTPage. */
typedef unsigned short u_short;

u_short GetTPage(int tp, int abr, int x, int y) {
    return (((tp & 0x3) << 7) | ((abr & 0x3) << 5) | ((y & 0x100) >> 4) |
            ((x & 0x3FF) >> 6) | ((y & 0x200) << 2));
}

static unsigned int GetTPage_alignment __attribute__((section(".text"))) = 0;
