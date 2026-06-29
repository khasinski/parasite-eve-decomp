void Task_SignalExit(void);

int Task_SignalSceneExit(void) {
    Task_SignalExit();
    return 1;
}
