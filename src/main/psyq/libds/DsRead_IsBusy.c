extern int g_DsReadBusy;

int DsRead_IsBusy(void);

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
