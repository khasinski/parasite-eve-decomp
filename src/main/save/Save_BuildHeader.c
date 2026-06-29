/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

typedef unsigned short u16;
typedef unsigned char u8;

extern u16 g_InvCategoryItemTable[];
extern int g_GameTimeTable;
extern int g_PlayTimeSeconds;
extern int g_PlayTimeFrameCounter;
extern u16 D_800C1EAC[];
extern int g_SavedDrawBlendColor;
extern short g_AyaMenuClampValue;
extern int g_SavePlayTimeMinutes;
extern int D_800C0DEC;
extern signed char g_SavedDrawBaseY;
extern u8 D_800C0DFD;
extern u8 D_800C0DFE;
extern short D_800C0E3C;
extern short g_CurrentMapNumber;

int Draw_GetBlendColor(void);
int Menu_GetItemContextFlag(void);
int Draw_GetBaseY(void);
unsigned int Menu_GetActionSubmenuSelection(void);
int MenuWidget_GetColumnLayoutMode(void);
int Save_ClassifyPlaytime(int arg0);
int Save_GetCurrentMapNumber(void);

void Save_BuildHeader(void) {
    u16 *dst;
    int i;
    int offset;
    unsigned int value;

    i = 0;
    dst = D_800C1EAC;
    offset = 0;
    while (i < 6) {
        *dst = *(u16 *)((char *)g_InvCategoryItemTable + offset);
        offset += 0x20;
        i++;
        dst++;
    }

    g_SavedDrawBlendColor = Draw_GetBlendColor();
    g_AyaMenuClampValue = Menu_GetItemContextFlag();
    g_SavePlayTimeMinutes = (unsigned int)g_PlayTimeSeconds / 60U;
    D_800C0DEC = (unsigned int)g_GameTimeTable / 60U;
    g_SavedDrawBaseY = Draw_GetBaseY();
    value = Menu_GetActionSubmenuSelection();
    D_800C0DFD = value | (MenuWidget_GetColumnLayoutMode() << 2);
    D_800C0DFE = 0;
    D_800C0E3C = Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
    g_CurrentMapNumber = Save_GetCurrentMapNumber();
}
