extern unsigned char g_CdStatus;

extern unsigned char g_CdMode;

extern unsigned char g_CdLastCom;

int CdStatus(void) {
    return g_CdStatus;
}

int CdMode(void) {
    return g_CdMode;
}

int CdLastCom(void) {
    return g_CdLastCom;
}
