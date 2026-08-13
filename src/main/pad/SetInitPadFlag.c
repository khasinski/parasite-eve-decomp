typedef struct PadInitFlagPage {
    int flag;
    char reserved04[0x4B50];
} PadInitFlagPage;

register PadInitFlagPage *g_PadInitFlagPage asm("$1");

void SetInitPadFlag(int value) {
    g_PadInitFlagPage = (PadInitFlagPage *)0x800A0000;
    g_PadInitFlagPage[-1].flag = value;
}
