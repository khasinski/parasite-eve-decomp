/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -G8 */
/*
 * Sony built this PsyQ 4.x TU with gcc 2.8.1 and a small-data threshold:
 * the retail epilogue has the jr-delay-slot fill (2.8.1) while both global
 * accesses are assembler macros (lw/sw sym expanded by GAS, $at store).
 * Under 2.8.1 that macro form only appears when the symbol is considered
 * small-data by cc1, hence the -G8 and the bounded declaration of the
 * callback table below (real table has 8 entries; the [2] bound only makes
 * its address materialize as an `la` macro instead of split %hi/%lo).
 */
extern int g_VSyncCount;
extern void (*g_IntrVSyncCallbackTable[2])(void);

void trapIntrVSync(void) {
    int i;

    g_VSyncCount++;
    for (i = 0; i < 8; i++) {
        if (g_IntrVSyncCallbackTable[i] != 0) {
            g_IntrVSyncCallbackTable[i]();
        }
    }
}

/*
 * The retail object carries an alternative-entry symbol (spimdisasm alabel)
 * pointing at the shared epilogue; reproduce it so the object's symbol
 * layout matches. No code is emitted by this block.
 */
__asm__(".globl func_80074464 # maspsx-keep\n"
        "\t.set func_80074464, trapIntrVSync+0x58 # maspsx-keep\n"
        "\t.type func_80074464, @function # maspsx-keep\n"
        "\t.size func_80074464, 0x14 # maspsx-keep\n"
        "\t.size trapIntrVSync, 0x58 # maspsx-keep\n");
