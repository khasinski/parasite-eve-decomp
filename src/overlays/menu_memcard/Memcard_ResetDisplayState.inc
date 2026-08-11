#ifndef MEMCARD_RESET_DISPLAY_STATE_NAME
#define MEMCARD_RESET_DISPLAY_STATE_NAME func_8012A848
#endif
#ifndef MEMCARD_DISPLAY_PTR0
#define MEMCARD_DISPLAY_PTR0 D_801D0DE8
#define MEMCARD_DISPLAY_PTR1 D_801D0DEC
#define MEMCARD_DISPLAY_PTR2 D_801D0DF0
#define MEMCARD_DISPLAY_PTR3 D_801D0DF4
#define MEMCARD_DISPLAY_PTR4 D_801D0DF8
#define MEMCARD_DISPLAY_PTR5 D_801D0DFC
#define MEMCARD_SLOT20_SOURCE D_801D1384
#define MEMCARD_SLOT92_SOURCE D_801D13AC
#endif

typedef struct MemcardRect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} MemcardRect;

typedef struct MemcardSlot20 {
    char bytes[0x14];
} MemcardSlot20;

typedef struct MemcardSlot92 {
    s32 words[0x17];
} MemcardSlot92;

extern u8 D_800B0DBA;
extern u32 D_800B0CD8;
extern void *MEMCARD_DISPLAY_PTR0;
extern void *MEMCARD_DISPLAY_PTR1;
extern void *MEMCARD_DISPLAY_PTR2;
extern void *MEMCARD_DISPLAY_PTR3;
extern void *MEMCARD_DISPLAY_PTR4;
extern void *MEMCARD_DISPLAY_PTR5;
extern MemcardSlot20 D_800BCE80[2];
extern MemcardSlot20 MEMCARD_SLOT20_SOURCE[2];
extern MemcardSlot92 D_800BCDC8[2];
extern MemcardSlot92 MEMCARD_SLOT92_SOURCE[2];

void DrawSync(s32 mode);
void VSync(s32 mode);
void SetDispMask(s32 enabled);
void func_8007512C(MemcardRect *rect, s32 x, s32 y);

void MEMCARD_RESET_DISPLAY_STATE_NAME(void) {
    s8 i;
    u8 *active;
    MemcardRect rect;

    active = &D_800B0DBA;
    if (*active != 0) {
        MEMCARD_DISPLAY_PTR0 = 0;
        MEMCARD_DISPLAY_PTR1 = 0;
        MEMCARD_DISPLAY_PTR5 = 0;
        MEMCARD_DISPLAY_PTR4 = 0;
        MEMCARD_DISPLAY_PTR2 = 0;
        MEMCARD_DISPLAY_PTR3 = 0;
        *active = 0;

        DrawSync(0);
        VSync(0);
        SetDispMask(0);

        i = 0;
        do {
            D_800BCE80[i] = MEMCARD_SLOT20_SOURCE[i];
            D_800BCDC8[i] = MEMCARD_SLOT92_SOURCE[i];
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

#undef MEMCARD_SLOT92_SOURCE
#undef MEMCARD_SLOT20_SOURCE
#undef MEMCARD_DISPLAY_PTR5
#undef MEMCARD_DISPLAY_PTR4
#undef MEMCARD_DISPLAY_PTR3
#undef MEMCARD_DISPLAY_PTR2
#undef MEMCARD_DISPLAY_PTR1
#undef MEMCARD_DISPLAY_PTR0
#undef MEMCARD_RESET_DISPLAY_STATE_NAME
