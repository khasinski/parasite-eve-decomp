void Task_GpuFlushPrimQueue(void);
void Scene_TickTimers(void);

extern char g_AnalogStickState[];

void MemCard_InitTransferState(char *arg0, char *arg1);
void MemCard_StartTimerTransfer(void);

void Boot_VsyncCallback(void) {
    Task_GpuFlushPrimQueue();
    Scene_TickTimers();
}

void Boot_InitMemCard(void) {
    MemCard_InitTransferState(g_AnalogStickState, g_AnalogStickState + 0x22);
    MemCard_StartTimerTransfer();
}
