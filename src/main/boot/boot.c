void Task_GpuFlushPrimQueue(void);
void Scene_TickTimers(void);

extern char g_AnalogStickState[];

void PadInitDirect(char *arg0, char *arg1);
void PadStartCom(void);

void Boot_VsyncCallback(void) {
    Task_GpuFlushPrimQueue();
    Scene_TickTimers();
}

void Boot_InitMemCard(void) {
    PadInitDirect(g_AnalogStickState, g_AnalogStickState + 0x22);
    PadStartCom();
}
