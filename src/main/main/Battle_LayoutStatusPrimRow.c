typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009E358_o __asm__("D_8009E358");
extern struct { char _[16]; } D_8009E360_o __asm__("D_8009E360");
extern struct { char _[16]; } D_8009E362_o __asm__("D_8009E362");
extern struct { char _[16]; } D_800B0E38_o __asm__("D_800B0E38");

#define g_ActiveDrawSlot (*(int *)&D_8009CDDC_o)
#define D_8009E358 ((u8 *)&D_8009E358_o)
#define D_8009E360 ((u8 *)&D_8009E360_o)
#define D_8009E362 ((u8 *)&D_8009E362_o)
#define D_800B0E38 ((u8 **)&D_800B0E38_o)

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))

void AddPrim(void *ot, void *prim);

void Battle_LayoutStatusPrimRow(int y) {
    int i;
    int slot;
    int geom_offset;
    u8 *prim;

    for (i = 0; i < 3; i++) {
        slot = g_ActiveDrawSlot;
        geom_offset = ((slot << 1) + slot) << 4;
        prim = D_8009E358 + geom_offset + i * 0x10;

        S16_AT(prim, 0x08) = U16_AT(D_8009E360, geom_offset) + i;
        S16_AT(prim, 0x0C) = 0x50 - i * 2;
        S16_AT(prim, 0x0E) = (s8)y - i * 2;
        S16_AT(prim, 0x0A) = U16_AT(D_8009E362, geom_offset) + i;

        AddPrim(D_800B0E38[slot] + (7 - i) * 4, prim);
    }
}
