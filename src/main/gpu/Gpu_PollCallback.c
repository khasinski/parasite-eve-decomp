#include "pe1/gpu_callbacks.h"

extern GpuCallbacks *g_GpuCallbacks;

int Gpu_PollCallback(void) {
    return g_GpuCallbacks->callback() < 0;
}
