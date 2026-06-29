/* MASPSX_FLAGS: --stack-return-delay */

int DsSync(int arg0);
int CdRom_GetPendingReadCount(void);

int Cd_GetReadyStatus(void) {
    int status = DsSync(0);

    if (status == 1 && CdRom_GetPendingReadCount() > 0) {
        status = 2;
    }

    return status;
}
