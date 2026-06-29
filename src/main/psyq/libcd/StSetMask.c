/* MASPSX_FLAGS: --store-return-delay */

extern int g_CdStreamMask;
extern int D_800B6918;
extern int g_CdStreamEndSector;

void StSetMask(int mask, int start, int end) {
    g_CdStreamMask = mask;
    D_800B6918 = start;
    g_CdStreamEndSector = end;
}
