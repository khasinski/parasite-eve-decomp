/* Psy-Q LIBGPU P26.OBJ: SetTile. */

void SetTile(unsigned char *arg0) {
    arg0[3] = 3;
    arg0[7] = 0x60;
}

static unsigned int SetTile_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };
