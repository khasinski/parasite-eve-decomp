typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u32 D_800B1644[];
extern s32 D_800A636C[];
extern u32 *D_800B0E38[];
extern s32 g_ActiveDrawSlot;

static s32 Render_TexturedQuadsNclip(u32 xy0, u32 xy1, u32 xy2) {
    s32 mac0;

    asm volatile("mtc2 %0,$12" : : "r"(xy0));
    asm volatile("mtc2 %0,$13" : : "r"(xy1));
    asm volatile("mtc2 %0,$14" : : "r"(xy2));
    asm volatile("nop");
    asm volatile("nop");
    asm volatile(".word 0x4B400006");
    asm volatile("swc2 $24,0(%0)" : : "r"(&mac0) : "memory");
    return mac0;
}

static s32 Render_TexturedQuadsDepth4(u16 a, u16 b, u16 c, u16 d) {
    return (D_800A636C[a] + D_800A636C[b] + D_800A636C[c] + D_800A636C[d]) >> 4;
}

static s32 Render_TexturedQuadsDepth3(u16 a, u16 b, u16 c) {
    return (D_800A636C[a] + D_800A636C[b] + D_800A636C[c]) / 12;
}

static void Render_TexturedQuadsLink(u8 *prim, s32 depth) {
    u32 *ot;
    u32 old;
    u32 prim_word;

    if (depth >= 0x1000) {
        return;
    }

    ot = D_800B0E38[g_ActiveDrawSlot] + depth;
    old = *ot;
    prim_word = U32_AT(prim, 0);
    U32_AT(prim, 0) = (prim_word & 0xFF000000) | (old & 0x00FFFFFF);
    *ot = (old & 0xFF000000) | ((u32)prim & 0x00FFFFFF);
}

static void Render_TexturedQuadsClear(u8 *prim) {
    U32_AT(prim, 0) &= 0xFF000000;
}

static void Render_TexturedQuadsStore4Wide(u8 *prim, u16 a, u16 b, u16 c, u16 d) {
    U32_AT(prim, 0x08) = D_800B1644[a];
    U32_AT(prim, 0x14) = D_800B1644[b];
    U32_AT(prim, 0x20) = D_800B1644[c];
    U32_AT(prim, 0x2C) = D_800B1644[d];
}

static void Render_TexturedQuadsStore3Wide(u8 *prim, u16 a, u16 b, u16 c) {
    U32_AT(prim, 0x08) = D_800B1644[a];
    U32_AT(prim, 0x14) = D_800B1644[b];
    U32_AT(prim, 0x20) = D_800B1644[c];
}

static void Render_TexturedQuadsStore4Tight(u8 *prim, u16 a, u16 b, u16 c, u16 d) {
    U32_AT(prim, 0x08) = D_800B1644[a];
    U32_AT(prim, 0x10) = D_800B1644[b];
    U32_AT(prim, 0x18) = D_800B1644[c];
    U32_AT(prim, 0x20) = D_800B1644[d];
}

static void Render_TexturedQuadsStore3Tight(u8 *prim, u16 a, u16 b, u16 c) {
    U32_AT(prim, 0x08) = D_800B1644[a];
    U32_AT(prim, 0x10) = D_800B1644[b];
    U32_AT(prim, 0x18) = D_800B1644[c];
}

static void Render_TexturedQuadsDraw4Wide(u8 **record_ptr, u8 **prim_ptr, int count) {
    u8 *record;
    u8 *prim_base;
    u8 *prim;
    int slot_offset;
    int i;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s32 depth;

    record = *record_ptr;
    prim_base = *prim_ptr;
    slot_offset = g_ActiveDrawSlot * 0x34;

    for (i = 0; i < count; i++, record += 0x0C, prim_base += 0x68) {
        a = U16_AT(record, 4);
        b = U16_AT(record, 6);
        c = U16_AT(record, 8);
        d = U16_AT(record, 10);
        prim = prim_base + slot_offset;

        if (Render_TexturedQuadsNclip(D_800B1644[a], D_800B1644[b], D_800B1644[c]) <= 0) {
            Render_TexturedQuadsClear(prim);
            continue;
        }

        depth = Render_TexturedQuadsDepth4(a, b, c, d);
        if (depth < 0x1000) {
            Render_TexturedQuadsLink(prim, depth);
            Render_TexturedQuadsStore4Wide(prim, a, b, c, d);
        }
    }

    *record_ptr = record;
    *prim_ptr = prim_base;
}

static void Render_TexturedQuadsDraw3Wide(u8 **record_ptr, u8 **prim_ptr, int count) {
    u8 *record;
    u8 *prim_base;
    u8 *prim;
    int slot_offset;
    int i;
    u16 a;
    u16 b;
    u16 c;
    s32 depth;

    record = *record_ptr;
    prim_base = *prim_ptr;
    slot_offset = g_ActiveDrawSlot * 0x28;

    for (i = 0; i < count; i++, record += 0x0C, prim_base += 0x50) {
        a = U16_AT(record, 4);
        b = U16_AT(record, 6);
        c = U16_AT(record, 8);
        prim = prim_base + slot_offset;

        if (Render_TexturedQuadsNclip(D_800B1644[a], D_800B1644[b], D_800B1644[c]) <= 0) {
            Render_TexturedQuadsClear(prim);
            continue;
        }

        depth = Render_TexturedQuadsDepth3(a, b, c);
        if (depth < 0x1000) {
            Render_TexturedQuadsLink(prim, depth);
            Render_TexturedQuadsStore3Wide(prim, a, b, c);
        }
    }

    *record_ptr = record;
    *prim_ptr = prim_base;
}

static void Render_TexturedQuadsDraw4Tight(u8 **record_ptr, u8 **prim_ptr, int count) {
    u8 *record;
    u8 *prim_base;
    u8 *prim;
    int slot_offset;
    int i;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s32 depth;

    record = *record_ptr;
    prim_base = *prim_ptr;
    slot_offset = g_ActiveDrawSlot * 0x24;

    for (i = 0; i < count; i++, record += 0x0C, prim_base += 0x48) {
        a = U16_AT(record, 4);
        b = U16_AT(record, 6);
        c = U16_AT(record, 8);
        d = U16_AT(record, 10);
        prim = prim_base + slot_offset;

        if (Render_TexturedQuadsNclip(D_800B1644[a], D_800B1644[b], D_800B1644[c]) <= 0) {
            Render_TexturedQuadsClear(prim);
            continue;
        }

        depth = Render_TexturedQuadsDepth4(a, b, c, d);
        if (depth < 0x1000) {
            Render_TexturedQuadsLink(prim, depth);
            Render_TexturedQuadsStore4Tight(prim, a, b, c, d);
        }
    }

    *record_ptr = record;
    *prim_ptr = prim_base;
}

static void Render_TexturedQuadsDraw3Tight(u8 **record_ptr, u8 **prim_ptr, int count) {
    u8 *record;
    u8 *prim_base;
    u8 *prim;
    int slot_offset;
    int i;
    u16 a;
    u16 b;
    u16 c;
    s32 depth;

    record = *record_ptr;
    prim_base = *prim_ptr;
    slot_offset = g_ActiveDrawSlot * 0x1C;

    for (i = 0; i < count; i++, record += 0x0C, prim_base += 0x38) {
        a = U16_AT(record, 4);
        b = U16_AT(record, 6);
        c = U16_AT(record, 8);
        prim = prim_base + slot_offset;

        if (Render_TexturedQuadsNclip(D_800B1644[a], D_800B1644[b], D_800B1644[c]) <= 0) {
            Render_TexturedQuadsClear(prim);
            continue;
        }

        depth = Render_TexturedQuadsDepth3(a, b, c);
        if (depth < 0x1000) {
            Render_TexturedQuadsLink(prim, depth);
            Render_TexturedQuadsStore3Tight(prim, a, b, c);
        }
    }

    *record_ptr = record;
    *prim_ptr = prim_base;
}

void Render_DrawTexturedQuads(u8 *object) {
    u8 *header;
    u8 *records;
    u8 *prims;
    int count;

    header = PTR_AT(object, 0);
    records = PTR_AT(object, 0x10);
    prims = PTR_AT(object, 0x54);

    count = U16_AT(header, 8);
    if (count > 0) {
        Render_TexturedQuadsDraw4Wide(&records, &prims, count);
    }

    count = U16_AT(header, 0x0A);
    if (count > 0) {
        Render_TexturedQuadsDraw3Wide(&records, &prims, count);
    }

    count = U16_AT(header, 0x0C);
    if (count > 0) {
        Render_TexturedQuadsDraw4Tight(&records, &prims, count);
    }

    count = U16_AT(header, 0x0E);
    if (count > 0) {
        Render_TexturedQuadsDraw3Tight(&records, &prims, count);
    }
}
