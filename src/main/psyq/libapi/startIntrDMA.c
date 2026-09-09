#include "pe1/psyq_api_internal.h"

DmaCallbackSetter startIntrDMA(void) {
    memclrIntrDMA((int *)g_IntrDmaHandlerTable, 8);
    *g_IntrDmaDispatchPtr = 0;
    InterruptCallback(3, trapIntrDMA);
    return setIntrDMA;
}
