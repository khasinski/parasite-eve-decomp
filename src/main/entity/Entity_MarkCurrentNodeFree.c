/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char *g_TaskNodePool;

int Entity_MarkCurrentNodeFree(void) {
    *(unsigned short *)(g_TaskNodePool + 8) |= 0x10;
    return 0;
}
