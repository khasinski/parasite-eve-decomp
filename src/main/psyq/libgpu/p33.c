/* Psy-Q LIBGPU P33.OBJ: SetDrawTPage. */

void SetDrawTPage(char *prim, int drawTexture, int dither, int tpage) {
    int code;
    int packedTpage;
    int *command = (int *)(prim + 4);

    prim[3] = 1;
    code = 0xE1000000;

    if (dither != 0) {
        code = 0xE1000200;
    }

    packedTpage = tpage & 0x9FF;
    if (drawTexture != 0) {
        packedTpage |= 0x400;
        *command = packedTpage | code;
        return;
    }

    *command = code | packedTpage;
}

static unsigned int SetDrawTPage_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
