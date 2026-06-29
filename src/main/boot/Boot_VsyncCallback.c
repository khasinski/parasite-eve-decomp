void Task_GpuFlushPrimQueue(void);
void Scene_TickTimers(void);

void Boot_VsyncCallback(void) {
    Task_GpuFlushPrimQueue();
    Scene_TickTimers();
}
