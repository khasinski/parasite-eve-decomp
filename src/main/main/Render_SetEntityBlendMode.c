typedef unsigned char u8;
typedef unsigned short u16;

extern int D_8009CDDC;

#define PTR_AT(ptr, off) (*(void **)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(short *)((u8 *)(ptr) + (off)))

#define SET_SEMI_TRANS(prim, enable) \
    do {                             \
        if ((enable) != 0) {         \
            U8_AT((prim), 7) |= 2;   \
        } else {                     \
            U8_AT((prim), 7) &= 0xFD;\
        }                            \
    } while (0)

void Render_SetEntityBlendMode(void *anim, int mode) {
    void *header;
    u8 *cmd;
    u8 *prim;
    int slot;
    int i;

    header = PTR_AT(anim, 0);
    slot = D_8009CDDC;
    if (header == 0) {
        return;
    }
    if (S16_AT(anim, 0xBA) == 0) {
        return;
    }

    cmd = PTR_AT(anim, 0x10);
    prim = PTR_AT(anim, 0x54);
    for (i = 0; i < U16_AT(header, 8); i++, cmd += 0xC, prim += 0x68) {
        SET_SEMI_TRANS(prim + (slot * 0x34), (U8_AT(cmd, 3) == 0xB) ? 1 : mode);
    }

    for (i = 0; i < U16_AT(header, 0xA); i++, cmd += 0xC, prim += 0x50) {
        SET_SEMI_TRANS(prim + (slot * 0x28), (U8_AT(cmd, 3) == 0x10) ? 1 : mode);
    }

    for (i = 0; i < U16_AT(header, 0xC); i++, cmd += 0xC, prim += 0x48) {
        SET_SEMI_TRANS(prim + (slot * 0x24), (U8_AT(cmd, 3) == 0x15) ? 1 : mode);
    }

    for (i = 0; i < U16_AT(header, 0xE); i++, cmd += 0xC, prim += 0x38) {
        SET_SEMI_TRANS(prim + (slot * 0x1C), (U8_AT(cmd, 3) == 0x1A) ? 1 : mode);
    }
}
