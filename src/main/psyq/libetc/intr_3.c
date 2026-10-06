/* PSY-Q LIBETC INTR, part 3 of 3: Sys_ClearWordBuf. */


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
