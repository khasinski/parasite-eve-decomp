
extern int D_800956EC;

typedef struct VideoModeDataPage {
    char reserved00[0x56EC];
    int video_mode;
} VideoModeDataPage;

register VideoModeDataPage *g_VideoModeWritePage asm("$1");

int SetVideoMode(int mode) {
    int old;

    old = D_800956EC;
    g_VideoModeWritePage = (VideoModeDataPage *)0x80090000;
    g_VideoModeWritePage->video_mode = mode;
    return old;
}
