/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef unsigned int u32;
typedef short s16;

extern u8 g_BattleSaveOverlayActive;
extern u8 g_MenuActiveMode;
extern u8 D_8009CE88;
extern u32 g_CurItemEffectData;

void Tbl_ResetAll(void);
int Save_GetMetadataWindowIndex(void);
void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3);
void Render_SetupColorTable(int arg0, int arg1, s16 *arg2);
void Draw_SetCursor(int arg0, int arg1);
void Draw_AllocColorGradient(int arg0, int arg1, int arg2, int arg3);

void Menu_SaveOverlayDraw(void) {
    s16 value;
    int x;
    int y;
    u32 d1f8;

    switch (g_BattleSaveOverlayActive) {
    case 1:
        value = -1;
        Tbl_ResetAll();
        if (Save_GetMetadataWindowIndex() != 0) {
            x = 0x14;
        } else {
            x = 0x61;
        }
        if (g_MenuActiveMode < 2) {
            y = 0xF;
        } else {
            y = 0xC3;
        }
        Menu_SetTextCursorRect(x, y, 0, 0);
        Render_SetupColorTable(0, 2, &value);
        asm volatile(
            "lbu\t$4,g_BattleSaveOverlayActive\n"
            "li\t$2,2\n"
            "sb\t$2,g_TextboxEntries\n"
            "li\t$2,75\n"
            "sb\t$2,D_8009CE88"
            :
            :
            : "$1", "$2", "$4", "memory");
        d1f8 = g_CurItemEffectData;
        asm volatile(
            "li\t$3,0x02000000\n"
            "sw\t$2,g_SaveMetadataTextPtr\n"
            "lw\t$2,D_800BCEB4\n"
            "addiu\t$4,$4,1\n"
            "sb\t$4,g_BattleSaveOverlayActive\n"
            "or\t$2,$2,$3\n"
            "sw\t$2,D_800BCEB4"
            :
            : "r"(d1f8)
            : "$1", "$3", "$4", "memory");
        break;
    case 2:
        if (D_8009CE88 == 0) {
            Tbl_ResetAll();
            g_BattleSaveOverlayActive = 0;
        }
        break;
    }

    D_8009CE88--;
    Draw_SetCursor(0, g_MenuActiveMode < 2 ? 0xB : 0xBF);
    Draw_AllocColorGradient(0x140, 0x14, 0, 0);
}
