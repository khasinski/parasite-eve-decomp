int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, int arg4);

int Task_SetCdRomScreenPos(unsigned short **arg0) {
    return CdRom_SetScreenPos(*arg0[0], *arg0[1], *arg0[2], *arg0[3], *arg0[4]), 1;
}
