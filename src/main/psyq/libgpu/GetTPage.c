typedef unsigned short u_short;

u_short GetTPage(int tp, int abr, int x, int y) {
    return (((tp & 0x3) << 7) | ((abr & 0x3) << 5) | ((y & 0x100) >> 4) |
            ((x & 0x3FF) >> 6) | ((y & 0x200) << 2));
}

static unsigned int GetTPage_alignment __attribute__((section(".text"))) = 0;


u_short GetClut(int x, int y) {
    return (y << 6) | ((x >> 4) & 0x3F);
}

static unsigned int tail_alignment_682BC[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000 };
