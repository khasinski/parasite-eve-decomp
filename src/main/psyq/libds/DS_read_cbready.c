/* CC1_VERSION: 2.8.1 */

extern int g_DsReadBusy;

void DsSyncCallback(int arg0);
void DsReadyCallback(int arg0);

void DS_read_cbready(void) {
    int *state;

    state = &g_DsReadBusy;
    asm volatile("" : "=r"(state) : "0"(state));
    if (*state == 1) {
        DsSyncCallback(state[-3]);
        DsReadyCallback(state[-2]);
    }
    *state = 0;
}
