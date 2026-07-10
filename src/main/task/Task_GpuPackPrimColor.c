int Task_GpuFlushPrimQueue(int start, int delta);

int Task_GpuPackPrimColor(int start, int end) {
    int delta = end - start;
    int step = Task_GpuFlushPrimQueue(start, delta) & 0xFFFF;

    return start + ((step * delta) >> 16);
}
