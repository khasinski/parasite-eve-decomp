
extern int g_GameStateFlags;
extern int g_SceneDispatchToken;
extern int g_PlayTimeFrameCounter;

void Str_EncodeBase32(void *out, int arg1);
void Sys_Shutdown(int *arg0);

int Task_EncodeBarcode(int **arg0) {
    int tmp[2];
    int value;
    int *counter;

    Str_EncodeBase32(&tmp[0], *arg0[0]);
    value = *arg0[0];
    g_GameStateFlags |= 0x2000;
    g_SceneDispatchToken = value;

    if (*arg0[0] == 0xA9400048) {
        counter = &g_PlayTimeFrameCounter;
        if (*counter == 0x7D0) {
            *counter = 0;
        } else {
            Sys_Shutdown(counter);
        }
    }
    return 0;
}
