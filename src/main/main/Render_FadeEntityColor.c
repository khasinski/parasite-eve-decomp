typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int D_8009CDDC;

#define W32(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define U8(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16(ptr, off) (*(short *)((u8 *)(ptr) + (off)))

void Render_FadeEntityColor(void *anim, int r, int g, int b) {
    void *header;
    u8 *prim;
    u32 color;
    int i;
    int slot;
    u8 opcode;

    header = *(void **)anim;
    slot = D_8009CDDC;
    if (header == 0) {
        return;
    }
    if (S16(anim, 0xBA) == 0) {
        return;
    }

    color = ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF);
    U8(anim, 0x90) = r;
    U8(anim, 0x91) = g;
    U8(anim, 0x92) = b;
    prim = *(u8 **)((u8 *)anim + 0x54);

    for (i = 0; i < U16(header, 8); i++, prim += 0x68) {
        u8 *p = prim + (slot * 0x34);
        opcode = U8(p, 7);
        W32(p, 4) = color;
        W32(p, 0x10) = color;
        W32(p, 0x1C) = color;
        W32(p, 0x28) = color;
        U8(p, 7) = opcode;
    }

    for (i = 0; i < U16(header, 0xA); i++, prim += 0x50) {
        u8 *p = prim + (slot * 0x28);
        opcode = U8(p, 7);
        W32(p, 4) = color;
        W32(p, 0x10) = color;
        W32(p, 0x1C) = color;
        U8(p, 7) = opcode;
    }

    for (i = 0; i < U16(header, 0xC); i++, prim += 0x48) {
        u8 *p = prim + (slot * 0x24);
        opcode = U8(p, 7);
        W32(p, 4) = color;
        W32(p, 0xC) = color;
        W32(p, 0x14) = color;
        W32(p, 0x1C) = color;
        U8(p, 7) = opcode;
    }

    for (i = 0; i < U16(header, 0xE); i++, prim += 0x38) {
        u8 *p = prim + (slot * 0x1C);
        opcode = U8(p, 7);
        W32(p, 4) = color;
        W32(p, 0xC) = color;
        W32(p, 0x14) = color;
        U8(p, 7) = opcode;
    }
}
