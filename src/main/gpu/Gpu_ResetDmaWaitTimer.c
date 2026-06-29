extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;

int VSync(int arg0);

void Gpu_ResetDmaWaitTimer(void) {
    g_GpuDmaTimeoutDeadline = VSync(-1) + 0xF0;
    g_GpuDmaWaitLoopCounter = 0;
}
