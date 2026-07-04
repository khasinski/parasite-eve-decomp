/* MASPSX_FLAGS: --stack-return-delay */

asm("GetDispEnv = GetDispEnv");

extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char g_GpuActiveDispEnv[];

void *GetDispEnv(void *arg0) {
    void *(*fn)(void *, const void *, unsigned int);

    fn = memcpy;
    fn(arg0, g_GpuActiveDispEnv, 0x14);
    return arg0;
}
