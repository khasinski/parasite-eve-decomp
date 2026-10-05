/* Unmatched semantic candidate: asm-differ Levenshtein score 1930.
 * The original uses a custom calling convention; see the adjacent notes. */
unsigned int Task_GpuFlushPrimQueue(void);

int Task_GpuPackPrimColor(int start, int end) {
    int delta = end - start;
    int step = Task_GpuFlushPrimQueue() & 0xFFFF;

    return start + (int)(((long long)step * delta) >> 16);
}
