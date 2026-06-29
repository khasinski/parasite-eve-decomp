void CdRom_SetSeekPos(unsigned int arg0);

int Task_SetCdRomSeekPos(unsigned short **arg0) {
    CdRom_SetSeekPos(**arg0);
    return 1;
}
