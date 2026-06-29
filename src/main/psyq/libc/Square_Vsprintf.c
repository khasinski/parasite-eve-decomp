void __maspsx_include_asm_hack_Square_Vsprintf(void) {
    asm(
        ".text # maspsx-keep\n"
        ".include \"src/main/psyq/libc/Square_Vsprintf.s\" # maspsx-keep\n"
    );
}
