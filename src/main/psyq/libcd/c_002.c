/* CC1_PROFILE: build0001 */
extern int g_CdStreamRingSize;
extern int g_CdStreamRingWritePtr;
extern int g_CdStreamRingReadSlot;
extern int D_800BE998;
extern int g_CdStreamDataReadyFlag;
extern int D_800B0CD0;
extern short g_StStreamState;
extern int g_StStreamReadState;

extern void init_ring_status(int arg0, int arg1);

void StClearRing(void) {
    int arg1 = g_CdStreamRingSize;

    g_CdStreamRingWritePtr = 0;
    g_CdStreamRingReadSlot = 0;
    D_800BE998 = 0;
    g_CdStreamDataReadyFlag = 0;
    init_ring_status(0, arg1);
    D_800B0CD0 = 0;
    g_StStreamState = 0;
    g_StStreamReadState = 0;
}
