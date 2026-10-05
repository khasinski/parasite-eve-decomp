#include "pe1/gpu_state.h"

int Gpu_PollCallback(void) {
    return D_80095744->callback() < 0;
}
