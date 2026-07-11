/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u16 D_800C1EAC[];
extern u16 D_800A1E6E[];
extern struct { char _[16]; } D_800C0DE8_o __asm__("D_800C0DE8");
extern struct { char _[16]; } D_800C0DEC_o __asm__("D_800C0DEC");
extern u8 D_800C0DFD_action[] __asm__("D_800C0DFD");
extern u8 D_800C0DFD_layout[] __asm__("D_800C0DFD");
extern struct { char _[16]; } D_800C0DFF_o __asm__("D_800C0DFF");
extern struct { char _[16]; } D_800C0E40_o __asm__("D_800C0E40");
extern struct { char _[16]; } D_800C0E44_o __asm__("D_800C0E44");
extern struct { char _[16]; } D_800A76A4_o __asm__("D_800A76A4");
extern struct { char _[16]; } D_800A76BC_o __asm__("D_800A76BC");
extern int D_8009D218;

#define D_800C0DE8 (*(int *)&D_800C0DE8_o)
#define D_800C0DEC (*(int *)&D_800C0DEC_o)
#define D_800C0DFF (*(s8 *)&D_800C0DFF_o)
#define g_AyaMenuClampValue (*(s16 *)&D_800C0E40_o)
#define D_800C0E44 ((u32 *)&D_800C0E44_o)
#define D_800A76A4 (*(int *)&D_800A76A4_o)
#define D_800A76BC (*(int *)&D_800A76BC_o)

void Draw_BlendColor(int color);
int Menu_ClampRange(int value);
int Draw_GetBaseY(void);
void Draw_SetBaseOffsetPosition(int x, int y);
void Menu_SetActionSubmenuSelection(int value);
void MenuWidget_SetColumnLayoutMode(int mode);
void Menu_CopyPromptCodes(u8 *src);

void Save_RestoreHeader(void) {
    register u16 *src asm("$4");
    register int i asm("$5");
    int offset;
    register u32 *saved_color asm("$16");
    u8 *prompt;

    src = D_800C1EAC;
    i = 0;
    offset = 0;
    while (i < 6) {
        *(u16 *)((u8 *)D_800A1E6E + offset) = *src++;
        i++;
        offset += 0x20;
    }

    saved_color = D_800C0E44;
    Draw_BlendColor(saved_color[0]);
    Menu_ClampRange(g_AyaMenuClampValue);

    D_800A76BC = (D_800C0DE8 * 15) * 4;
    D_800A76A4 = (D_800C0DEC * 15) * 4;
    Draw_SetBaseOffsetPosition(0, D_800C0DFF - Draw_GetBaseY());
    Menu_SetActionSubmenuSelection(D_800C0DFD_action[0] & 3);
    MenuWidget_SetColumnLayoutMode((D_800C0DFD_layout[0] >> 2) & 1);

    prompt = (u8 *)saved_color - 0x64;
    if (D_8009D218 != 0) {
        prompt = (u8 *)saved_color - 0x54;
    }
    Menu_CopyPromptCodes(prompt);
}
