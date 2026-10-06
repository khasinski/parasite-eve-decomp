/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_2.OBJ, part 1 of 5: CQ_clear_queue. */
/* DSSYS_2 is split where its functions need different compilers: parts 2
 * (CQ_delete_command) and 4 (DsInit) only match with GCC 2.8.1, the rest
 * with GCC 2.7.2. */

void CQ_clear_queue(void *arg0) {
    int i;
    char *ptr;
    char *bytes = arg0;
    int *words = arg0;

    words[0] = 0;
    i = 3;
    ptr = bytes + 3;
    bytes[4] = 0;
    for (; i >= 0; i--) {
        ptr[5] = 0;
        ptr--;
    }
    words[3] = 0;
    words[4] = 0;
    words[5] = 0;
}
