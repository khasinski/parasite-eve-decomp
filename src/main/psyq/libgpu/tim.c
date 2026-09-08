typedef struct TimRect {
    short x;
    short y;
    short w;
    short h;
} TimRect;

typedef struct TimBlock {
    int length;
    TimRect rect;
    int pixels[1];
} TimBlock;

typedef struct TimFile {
    int magic;
    int flags;
    TimBlock first_block;
} TimFile;

typedef union TimBlockAddress {
    TimBlock *block;
    unsigned long address;
} TimBlockAddress;

int LoadImage(TimRect *rect, int *pixels);

int *Gpu_LoadTimImage(TimFile *tim) {
    TimBlock *clut;
    TimBlock *image;
    int *pixels;

    clut = 0;
    if ((tim->flags & 8) != 0) {
        TimBlockAddress next;

        clut = &tim->first_block;
        next.block = clut;
        next.address += clut->length;
        image = next.block;
    } else {
        image = &tim->first_block;
    }

    pixels = image->pixels;
    LoadImage(&image->rect, pixels);
    if (clut != 0) {
        LoadImage(&clut->rect, clut->pixels);
    }
    return pixels;
}

int Str_GetTableEntryA(int arg0) {
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        return arg0 + 0xC;
    }
    return 0;
}

void *Widget_GetDataPtr(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return base + 4;
}

int Str_GetTableEntryB(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return (int)(base + 0xC);
}

int Str_GetTableEntryC(int arg0) {
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        return arg0 + 0x14;
    }
    return 0;
}
