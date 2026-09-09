void SetPolyF3(unsigned char *arg0) {
    arg0[3] = 4;
    arg0[7] = 0x20;
}

static unsigned int SetPolyF3_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetPolyG3(unsigned char *arg0) {
    arg0[3] = 6;
    arg0[7] = 0x30;
}

static unsigned int SetPolyG3_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetPolyFT4(unsigned char *arg0) {
    arg0[3] = 9;
    arg0[7] = 0x2C;
}

static unsigned int SetPolyFT4_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetPolyG4(unsigned char *arg0) {
    arg0[3] = 8;
    arg0[7] = 0x38;
}

static unsigned int SetPolyG4_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetPolyGT4(unsigned char *arg0) {
    arg0[3] = 0xC;
    arg0[7] = 0x3C;
}

static unsigned int SetPolyGT4_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetSprt(unsigned char *arg0) {
    arg0[3] = 4;
    arg0[7] = 0x64;
}

static unsigned int SetSprt_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetTile1(unsigned char *arg0) {
    arg0[3] = 2;
    arg0[7] = 0x68;
}

static unsigned int SetTile1_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetTile(unsigned char *arg0) {
    arg0[3] = 3;
    arg0[7] = 0x60;
}

static unsigned int SetTile_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetLineF2(unsigned char *arg0) {
    arg0[3] = 3;
    arg0[7] = 0x40;
}

static unsigned int SetLineF2_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };

void SetDrawMode(char *prim, int drawTexture, int dither, int tpage) {
    int code;
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

static unsigned int SetDrawMode_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
