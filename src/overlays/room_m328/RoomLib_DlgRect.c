int RoomLib_DlgRect(void) {
    __asm__ volatile(
        "nop\n\t"
        ".word 0x0000FFF0");
    return 0;
}
