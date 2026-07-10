typedef unsigned char u8;
typedef unsigned short u16;

typedef struct StatusSymbolTable {
    u8 data[0x12];
} StatusSymbolTable;

extern StatusSymbolTable D_80010DFC;
extern StatusSymbolTable D_80010E10;
extern StatusSymbolTable D_80010E24;
extern int D_8009CDDC;
extern u8 D_8009E930[];
extern u8 D_8009E93C[];
extern u8 D_8009E93D[];
extern u16 D_8009E360[];
extern u16 D_8009E362[];
extern void *D_800B0E38[];

void AddPrim(void *ot, void *prim);

void Battle_DrawStatusSymbol(int id) {
    StatusSymbolTable table0;
    StatusSymbolTable table1;
    StatusSymbolTable table2;
    volatile u8 stack_pad[8];
    int byte_offset;
    int buf;
    int prim_offset;
    u8 *prim;
    register u8 *prim_base asm("$9");
    u8 *entry0;
    u8 *entry1;
    register u8 *entry2 asm("$6");
    int geom_offset;

    table0 = D_80010DFC;
    table1 = D_80010E10;
    table2 = D_80010E24;

    byte_offset = ((signed char)id) * 2;
    entry0 = table0.data + byte_offset;
    entry1 = table1.data + byte_offset;
    entry2 = table2.data + byte_offset;
    prim_base = D_8009E930;

    buf = D_8009CDDC;
    prim_offset = ((buf << 3) - buf) << 2;
    D_8009E93C[prim_offset] = entry0[0];

    buf = D_8009CDDC;
    prim_offset = ((buf << 3) - buf) << 2;
    D_8009E93D[prim_offset] = entry0[1];

    buf = D_8009CDDC;
    prim_offset = ((buf << 3) - buf) << 2;
    prim = prim_base + prim_offset;
    *(u16 *)(prim + 0x10) = entry1[0];
    *(u16 *)(prim + 0x12) = entry1[1];

    geom_offset = ((buf << 1) + buf) << 4;
    *(u16 *)(prim + 8) = D_8009E360[geom_offset / 2] + entry2[0];
    *(u16 *)(prim + 0xA) = D_8009E362[geom_offset / 2] + entry2[1];

    AddPrim((u8 *)D_800B0E38[buf] + 0x10, prim - 8);
}
