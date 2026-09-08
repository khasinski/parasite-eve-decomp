extern void (*g_IntrDmaHandlerTable[])(void);
extern int *g_IntrDmaDispatchPtr;

void memclrIntrDMA(int *ptr, int count);
void trapIntrDMA(void);
void *setIntrDMA(int channel, void *callback);
void InterruptCallback(int event, void (*callback)(void));

void *startIntrDMA(void) {
    memclrIntrDMA((int *)g_IntrDmaHandlerTable, 8);
    *g_IntrDmaDispatchPtr = 0;
    InterruptCallback(3, trapIntrDMA);
    return setIntrDMA;
}
