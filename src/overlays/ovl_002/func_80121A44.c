typedef signed char s8;
typedef unsigned char u8;
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
extern u32 D_800B0CD8;
extern void *D_80122420;
extern void *D_80122424;
extern void *D_80122428 asm(".L80122428");
extern void *D_8012242C;
extern void *D_80122430 asm(".L80122430");
extern void *D_80122434;
extern Overlay002Slot20 D_800BCE80[2];
extern Overlay002Slot20 D_801227EC[2];
extern Overlay002Slot92 D_800BCDC8[2];
extern Overlay002Slot92 D_80122814[2];

void DrawSync(s32 arg0);
void VSync(s32 arg0);
void SetDispMask(s32 arg0);
void func_8007512C(Overlay002Rect *rect, s32 arg1, s32 arg2);

void func_80121A44(void) {
    s8 i;
    u8 *active;
    Overlay002Rect rect;

    active = &D_800B0DBA;
    if (*active != 0) {
        D_80122420 = 0;
        D_80122424 = 0;
        D_80122434 = 0;
        D_80122430 = 0;
        D_80122428 = 0;
        D_8012242C = 0;
        *active = 0;

        DrawSync(0);
        VSync(0);
        SetDispMask(0);

        i = 0;
        do {
            D_800BCE80[i] = D_801227EC[i];
            D_800BCDC8[i] = D_80122814[i];
            i++;
        } while (i < 2);

        rect.x = 0x200;
        rect.y = 0;
        rect.w = 0xC0;
        rect.h = 0x100;
        func_8007512C(&rect, 0x140, 0);

        if ((D_800B0CD8 & 0x8000000) == 0) {
            rect.x = 0x200;
            rect.y = 0x100;
            rect.w = 0x140;
            rect.h = 0x40;
            func_8007512C(&rect, 0, 0x1C0);
        }
    }
}
