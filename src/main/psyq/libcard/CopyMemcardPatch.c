/* ASSEMBLER: GNU */
/* Psy-Q _copy_memcard_patch. Retains the existing link symbol for callers.
 * Empty constraints preserve the retail schedule; see proposals/PsyqQueueFour. */
/* Treat the patch instruction image as words; copy up to the next label. */
extern const unsigned func_8007E344[], func_8007E3B4[];
void func_8007E4E0(void) {
    register unsigned *dst = (unsigned *)0xdf80;
    register const unsigned *src asm("$10") = func_8007E344;
    register const unsigned *end asm("$9") = func_8007E3B4;
    register unsigned value asm("$3");
    do {
        value = *src;
        asm("" : "=r"(value) : "0"(value));
        *dst = value;
        asm("" ::: "memory");
        src++;
        dst++;
    } while (src != end);
}
