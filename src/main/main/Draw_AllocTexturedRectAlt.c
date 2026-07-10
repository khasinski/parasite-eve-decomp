typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct TexturedRectPrim {
    u32 tag;
    u8 r0, g0, b0, code;
    u16 x0, y0;
    u8 u0, v0;
    u16 clut;
    u16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    u16 x2, y2;
    u8 u2, v2;
    u16 pad2;
    u16 x3, y3;
    u8 u3, v3;
    u16 pad3;
} TexturedRectPrim;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;
extern int *D_8009D12C;
extern int D_8009D124;
extern int D_8009D128;
extern int D_800A2270[];
extern int D_800A22B0[];

u8 *Draw_LookupGlyphDescriptor(int glyph);
void Draw_PrintNumberWidth2Unk(int value);
void Draw_AllocSprite(int glyph);
void SetDrawMode(void *packet, int dfe, int dtd, int tpage);
void BoundsCheck_AssertStub(int arg0);

#define PUSH_CURSOR()                                                                                                  \
    do {                                                                                                              \
        int *cursor_stack = D_8009D12C;                                                                               \
        if ((u32)cursor_stack < (u32)D_800A22B0) {                                                                    \
            cursor_stack[0] = D_8009D124;                                                                             \
            cursor_stack[1] = D_8009D128;                                                                             \
            D_8009D12C = cursor_stack + 2;                                                                            \
        } else {                                                                                                      \
            BoundsCheck_AssertStub(2);                                                                                \
        }                                                                                                             \
    } while (0)

#define POP_CURSOR()                                                                                                   \
    do {                                                                                                              \
        int *cursor_stack = D_8009D12C;                                                                               \
        if ((u32)D_800A2270 < (u32)cursor_stack) {                                                                    \
            D_8009D12C = cursor_stack - 2;                                                                            \
            D_8009D124 = cursor_stack[-2];                                                                            \
            D_8009D128 = cursor_stack[-1];                                                                            \
        } else {                                                                                                      \
            BoundsCheck_AssertStub(3);                                                                                \
        }                                                                                                             \
    } while (0)

#define ALLOC_PACKET(packet, size)                                                                                     \
    do {                                                                                                              \
        u8 *old = D_8009D100;                                                                                         \
        u8 *next = old + (size);                                                                                      \
        packet = 0;                                                                                                   \
        if (next < D_8009D104 + 0x4000) {                                                                             \
            D_8009D100 = next;                                                                                        \
            packet = (void *)old;                                                                                     \
        } else {                                                                                                      \
            BoundsCheck_AssertStub(1);                                                                                \
        }                                                                                                             \
    } while (0)

#define LINK_PACKET(packet)                                                                                            \
    do {                                                                                                              \
        u32 *ot = D_8009D11C;                                                                                         \
        (packet)->tag = ((packet)->tag & 0xFF000000) | (*ot & 0x00FFFFFF);                                            \
        *ot = (*ot & 0xFF000000) | ((u32)(packet) & 0x00FFFFFF);                                                      \
    } while (0)

#define EMIT_SEGMENT(x_left, x_right, u_add)                                                                           \
    do {                                                                                                              \
        TexturedRectPrim *prim;                                                                                       \
        int y0;                                                                                                       \
        ALLOC_PACKET(prim, sizeof(TexturedRectPrim));                                                                 \
        if (prim != 0) {                                                                                              \
            *(u32 *)&prim->r0 = D_8009D10C != 0 ? D_8009D114 : D_8009D110;                                            \
            ((u8 *)prim)[3] = 9;                                                                                      \
            prim->code = 0x2C;                                                                                        \
        }                                                                                                             \
        prim->x0 = (x_left);                                                                                          \
        y0 = D_8009D128 + 1;                                                                                          \
        prim->y1 = y0;                                                                                                \
        prim->y0 = y0;                                                                                                \
        prim->x2 = (x_left);                                                                                          \
        prim->x3 = (x_right);                                                                                         \
        prim->x1 = (x_right);                                                                                         \
        y0 += desc[5];                                                                                                \
        prim->y3 = y0;                                                                                                \
        prim->y2 = y0;                                                                                                \
        prim->u2 = desc[0] + (u_add);                                                                                 \
        prim->u0 = prim->u2;                                                                                          \
        prim->v1 = desc[1];                                                                                           \
        prim->v0 = prim->v1;                                                                                          \
        prim->u3 = prim->u0;                                                                                          \
        prim->u1 = prim->u0;                                                                                          \
        y0 = prim->v0 + desc[5];                                                                                      \
        prim->v3 = y0;                                                                                                \
        prim->v2 = y0;                                                                                                \
        prim->clut = *(u16 *)(desc + 2);                                                                              \
        prim->tpage = 7;                                                                                              \
        LINK_PACKET(prim);                                                                                            \
    } while (0)

void Draw_AllocTexturedRectAlt(int value, int width) {
    u8 *desc;
    void *packet;

    desc = Draw_LookupGlyphDescriptor(0x48);

    PUSH_CURSOR();
    PUSH_CURSOR();
    D_8009D124 += 0x27;
    D_8009D128 -= 2;
    Draw_PrintNumberWidth2Unk(value);
    D_8009D110 = 0x808080;
    D_8009D114 = 0x404040;
    POP_CURSOR();

    Draw_AllocSprite(0x49);

    if (width < 0x2F) {
        EMIT_SEGMENT(D_8009D124 + 1, D_8009D124 + 2, 0);
    }
    if (width < 0x2E) {
        EMIT_SEGMENT(D_8009D124 + 2, D_8009D124 + 0x30 - width, 1);
    }
    if (width < 0x30) {
        EMIT_SEGMENT(D_8009D124 + 0x30 - width, D_8009D124 + 0x31 - width, 2);
    }
    if (width > 0) {
        EMIT_SEGMENT(D_8009D124 + 0x31 - width, D_8009D124 + 0x32 - width, 3);
    }
    if (width >= 2) {
        EMIT_SEGMENT(D_8009D124 + 0x32 - width, D_8009D124 + 0x31, 4);
        EMIT_SEGMENT(D_8009D124 + 0x31, D_8009D124 + 0x32, 5);
    }

    POP_CURSOR();

    ALLOC_PACKET(packet, 8);
    if (packet != 0) {
        SetDrawMode(packet, 0, 0, ((desc[6] & 3) << 7) | 7);
    }
    LINK_PACKET((TexturedRectPrim *)packet);
}
