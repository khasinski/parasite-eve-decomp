#include "pe1/psyq_callbacks.h"

void Gpu_SetDisplayBuffer(void);

void Gpu_RestoreDmaCallback(void) {
    DMACallback(2, Gpu_SetDisplayBuffer);
}
