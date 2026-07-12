extern int g_IntrDmaHandlerTable[];
extern int *g_IntrDmaDispatchPtr;

void memclrIntrDMA(int *ptr, int count);
void Render_SetupSceneBuffers(void);
void setIntrDMA(void);
void InterruptCallback(int event, void (*callback)(void));

void *startIntrDMA(void) {
    memclrIntrDMA(g_IntrDmaHandlerTable, 8);
    *g_IntrDmaDispatchPtr = 0;
    InterruptCallback(3, Render_SetupSceneBuffers);
    return setIntrDMA;
}
