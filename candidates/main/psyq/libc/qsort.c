/* Best stock-pipeline candidate: 98.10% vs retail. All 84 instructions match
 * except the epilogue: retail fills the jr delay slot with the stack restore
 * (ASPSX reorder behavior); stock maspsx emits addiu; jr; nop. */
extern void Mem_SwapBuffers(void *, void *, unsigned int);
void qsort(void *base, unsigned int n, unsigned int size, int (*cmp)())
{
    char *b = base;
    char *j = b + size;
    unsigned int nlo = 0;
    char *lo;
    unsigned int i;
    if (n < 2) return;
    if (n == 2) {
        if (cmp(b, j) > 0) Mem_SwapBuffers(b, j, size);
        return;
    }
    Mem_SwapBuffers(b, b + (n >> 1) * size, size);
    lo = b;
    i = 1;
    while (i < n) {
        if (cmp(j, b) < 0) {
            lo += size;
            nlo++;
            if (j != lo) Mem_SwapBuffers(j, lo, size);
        }
        i++;
        j += size;
    }
    if (lo != b) Mem_SwapBuffers(b, lo, size);
    qsort(b, nlo, size, cmp);
    qsort(lo + size, n - nlo - 1, size, cmp);
}
