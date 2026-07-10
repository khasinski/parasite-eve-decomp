void SetDrawMode(char *prim, int drawTexture, int dither, int tpage) {
    register int code asm("$3");
    register int packedTpage asm("$2");

    prim[3] = 1;
    code = 0xE1000000;

    if (dither != 0) {
        code = 0xE1000200;
    }

    packedTpage = tpage & 0x9FF;
    if (drawTexture != 0) {
        packedTpage |= 0x400;
    }

    *(int *)(prim + 4) = code | packedTpage;
}
