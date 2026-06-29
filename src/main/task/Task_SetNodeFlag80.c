/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char *g_TaskNodePool;

int Task_SetNodeFlag80(void) {
    *(unsigned short *)(g_TaskNodePool + 8) |= 0x80;
    return 1;
}
