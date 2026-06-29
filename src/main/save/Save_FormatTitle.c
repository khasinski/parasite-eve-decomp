typedef unsigned char u8;

extern u8 g_SaveTitleBuffer[];
extern int g_SaveTitleStyleFlag;
extern int D_800A1708;
extern int D_800A170C;
extern int D_800A1710;
extern int D_800A1714;
extern int D_800A1718;
extern int D_800A171C;
extern int g_PlayTimeFrameCounter;
extern char *g_SaveTitleFormatLongPtr;
extern char *g_SaveTitleFormatShortPtr;

void bzero(void *dst, int len);
void Save_SprintfSjis(u8 *dst, char *fmt);
int Save_ClassifyPlaytime(int arg0);
int Save_GetCurrentMapNumber(void);
u8 *Tbl_FindNthNonEmpty(int arg0);

u8 *Save_FormatTitle(int chapter, int playtime_seconds) {
    int minutes_total;
    int hours;
    int minutes;
    int seconds;
    u8 *value;

    bzero(g_SaveTitleBuffer, 0x44);

    hours = playtime_seconds / 3600;
    minutes_total = playtime_seconds / 60;
    minutes = minutes_total - (hours * 60);
    seconds = playtime_seconds - (minutes_total * 60);

    D_800A1708 = chapter;
    D_800A170C = hours;
    D_800A1710 = minutes;
    D_800A1714 = seconds;

    if (g_SaveTitleStyleFlag != 0) {
        value = Tbl_FindNthNonEmpty(-1);
        D_800A1718 = (int)value;
        Save_SprintfSjis(g_SaveTitleBuffer, g_SaveTitleFormatShortPtr);
    } else {
        D_800A1718 = Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
        value = Tbl_FindNthNonEmpty(Save_GetCurrentMapNumber());
        D_800A171C = (int)value;
        Save_SprintfSjis(g_SaveTitleBuffer, g_SaveTitleFormatLongPtr);
    }

    return g_SaveTitleBuffer;
}
