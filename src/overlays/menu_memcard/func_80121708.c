typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

typedef struct Overlay002Rect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Overlay002Rect;

typedef struct Overlay002Slot20 {
    char bytes[0x14];
} Overlay002Slot20;

typedef struct Overlay002Slot92 {
    s32 words[0x17];
} Overlay002Slot92;

extern u8 D_800B0DBA;
extern u16 D_800B0DBC;
extern u8 D_800B0DBE;
extern u32 D_800B0CD8;
extern void *D_80122420;
extern void *D_80122424;
extern void *D_80122428 asm(".L80122428");
extern void *D_8012242C;
extern void *D_80122430 asm(".L80122430");
extern void *D_80122434;
extern s16 D_801227E8;
extern Overlay002Slot20 D_800BCE80[2];
extern Overlay002Slot20 D_801227EC[2];
extern Overlay002Slot92 D_800BCDC8[2];
extern Overlay002Slot92 D_80122814[2];

void func_8007512C(Overlay002Rect *rect, s32 arg1, s32 arg2);

s32 func_80121708(s32 count, u8 **resources) {
    s8 i;
    s32 width;
    u8 *first_base;
    u8 *base;
    u8 *slot24;
    u8 *slot34;
    u8 *slot28;
    Overlay002Rect rect;

    if ((u8)(count - 1) < 2U) {
        for (i = 0; i < count; i++) {
            if (resources[i] == 0) {
                goto draw_second;
            }
        }

        if (D_800B0DBA != 0) {
            return 0;
        }

        if ((s8)count == 1) {
            first_base = resources[0];
            slot24 = first_base + 0xFA00;
            slot34 = first_base + 0x1F400;
            D_80122428 = first_base + 0x50400;
            D_80122420 = first_base;
            D_80122424 = slot24;
            D_80122434 = slot34;
            D_80122430 = first_base + 0x3F400;
            slot34 = first_base + 0x53100;
            base = *(u8 **)(slot24 + 4);
            slot24 = D_80122430;
        } else if ((s8)count == 2) {
            first_base = resources[0];
            slot24 = first_base + 0xFA00;
            slot34 = first_base + 0x1F400;
            base = resources[1];
        } else {
            first_base = 0;
            base = 0;
            slot24 = 0;
            slot34 = 0;
        }

        D_80122420 = first_base;
        D_80122424 = slot24;
        D_80122434 = slot34;
        slot28 = base + 0x11000;
        D_80122428 = slot28;
        D_80122430 = base;
        D_8012242C = base + 0x13D00;

        D_800B0DBA = 1;
        D_800B0DBE = 0x98;
        D_800B0DBC = 0;
        D_801227E8 = -1;

        i = 0;
        do {
            D_801227EC[i] = D_800BCE80[i];
            D_80122814[i] = D_800BCDC8[i];
            i++;
        } while (i < 2);

        rect.x = 0x140;
        rect.y = 0;
        rect.w = 0xC0;
        width = 0x140;
        rect.h = 0x100;
        func_8007512C(&rect, 0x200, 0);
    }

draw_second:
    if ((D_800B0CD8 & 0x8000000) == 0) {
        rect.x = 0;
        rect.y = 0x1C0;
        rect.w = width;
        rect.h = 0x40;
        func_8007512C(&rect, 0x200, 0x100);
    }

    return 1;
}
