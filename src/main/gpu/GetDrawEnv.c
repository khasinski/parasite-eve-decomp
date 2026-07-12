/* MASPSX_FLAGS: --stack-return-delay */

extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char D_8009575C[];

void *GetDrawEnv(void *arg0) {
    void *(*fn)(void *, const void *, unsigned int);

    fn = memcpy;
    fn(arg0, D_8009575C, 0x5C);
    return arg0;
}
