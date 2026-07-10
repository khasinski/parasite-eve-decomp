extern char D_800C0DE0[];

char *Save_GetActiveMetadataBuffer(void) {
    register int isSecondBuffer asm("$2");
    char *buffer;

    asm volatile("lw %0,0x4A8($gp)" : "=r"(isSecondBuffer));
    buffer = D_800C0DE0;

    if (isSecondBuffer != 0) {
        buffer += 0x10;
    }

    return buffer;
}
