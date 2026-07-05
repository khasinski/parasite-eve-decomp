

void Sys_ClearWordBuf(int *dst, int count) {
    int i = count - 1;

    if (count != 0) {
        do {
            *dst = 0;
            i--;
            dst++;
        } while (i != -1);
    }
}

void __maspsx_include_asm_hack_RawData_80074354(void) {
    asm(
        ".text # maspsx-keep\n"
        ".include \"src/main/sys/RawData_80074354.inc.s\" # maspsx-keep\n"
    );
}
