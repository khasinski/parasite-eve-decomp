/* Psy-Q LIBGPU P01.OBJ: GetClut. */
typedef unsigned short u_short;

u_short GetClut(int x, int y) {
    return (y << 6) | ((x >> 4) & 0x3F);
}

static unsigned int GetClut_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000 };
