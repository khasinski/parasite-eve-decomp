/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int Task_GpuFlushPrimQueue(void);
int Task_GpuPackPrimColor(void);

int Task_SelectAudio(int **arg0) {
    register int lhs asm("$4");
    register int rhs asm("$5");

    lhs = *arg0[1];
    rhs = *arg0[2];
    if (lhs == rhs) {
        *arg0[0] = Task_GpuFlushPrimQueue();
    } else {
        *arg0[0] = Task_GpuPackPrimColor();
    }

    return 1;
}
