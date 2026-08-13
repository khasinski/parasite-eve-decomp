
typedef struct CdStreamMaskPage {
    char reserved00[0xDC0];
    int mask;
} CdStreamMaskPage;

typedef struct CdStreamStartPage {
    char reserved00[0x6918];
    int start;
} CdStreamStartPage;

typedef struct CdStreamEndPage {
    char reserved00[0xDBC];
    int end;
} CdStreamEndPage;

register void *g_CdStreamWritePage asm("$1");

void StSetMask(int mask, int start, int end) {
    g_CdStreamWritePage = (void *)0x800C0000;
    ((CdStreamMaskPage *)g_CdStreamWritePage)->mask = mask;
    g_CdStreamWritePage = (void *)0x800B0000;
    ((CdStreamStartPage *)g_CdStreamWritePage)->start = start;
    g_CdStreamWritePage = (void *)0x800C0000;
    ((CdStreamEndPage *)g_CdStreamWritePage)->end = end;
}
