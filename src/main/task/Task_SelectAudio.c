int Task_GpuFlushPrimQueue(void);
int Task_GpuPackPrimColor(int start, int end);

/* Equal bounds return one raw RNG word. Otherwise the result is in [start, end). */
int Task_Random(int **arg0) {
    int start = *arg0[1];
    int end = *arg0[2];

    if (start == end) {
        *arg0[0] = Task_GpuFlushPrimQueue();
    } else {
        *arg0[0] = Task_GpuPackPrimColor(start, end);
    }

    return 1;
}
