void MenuWidget_ClearColumnLayout(void *ptr) {
    *(int *)((char *)ptr + 0x70) = -1;
    *(int *)((char *)ptr + 0x44) = -1;
}
