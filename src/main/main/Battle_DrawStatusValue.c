typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009E360_o __asm__("D_8009E360");
extern struct { char _[16]; } D_8009E362_o __asm__("D_8009E362");
extern struct { char _[16]; } D_8009E888_o __asm__("D_8009E888");
extern struct { char _[16]; } D_8009E8B8_o __asm__("D_8009E8B8");
extern struct { char _[16]; } D_800B0E38_o __asm__("D_800B0E38");

#define g_ActiveDrawSlot (*(int *)&D_8009CDDC_o)
#define D_8009E360 ((u8 *)&D_8009E360_o)
#define D_8009E362 ((u8 *)&D_8009E362_o)
#define D_8009E888 ((u8 *)&D_8009E888_o)
#define D_8009E8B8 ((u8 *)&D_8009E8B8_o)
#define D_800B0E38 ((u8 **)&D_800B0E38_o)

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))

void AddPrim(void *ot, void *prim);
void Battle_DrawDecimalNumber(void *base, int x, int y, int value, int use_status_colour);

void Battle_DrawStatusValue(int value, int y_offset) {
    int slot;
    int geom_offset;
    int prim_offset;
    int y;
    u8 *prim;

    slot = g_ActiveDrawSlot;
    geom_offset = ((slot << 1) + slot) << 4;
    prim_offset = ((slot << 3) - slot) << 2;

    prim = D_8009E888 + prim_offset;
    y = U16_AT(D_8009E362, geom_offset) + y_offset;
    S16_AT(prim, 0x08) = U16_AT(D_8009E360, geom_offset) + 8;
    S16_AT(prim, 0x0A) = y;
    AddPrim(D_800B0E38[slot] + 0x10, prim - 8);

    y = (s16)(y - 1);
    slot = g_ActiveDrawSlot;
    geom_offset = ((slot << 1) + slot) << 4;
    Battle_DrawDecimalNumber(
        D_8009E8B8 + (((slot << 3) - slot) << 3),
        (s16)(U16_AT(D_8009E360, geom_offset) + 0x40),
        y,
        (s16)value,
        1);
}
