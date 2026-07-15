int RoomLib_DlgRect(void) {
    __asm__ volatile(
        "nop\n\t"
        ".word 0x0000FFF0\n\t"
        ".word 0x00000400\n\t"
        ".word 0x00010000");
    return 0;
}
