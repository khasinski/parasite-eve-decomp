extern unsigned char g_GpuControlRegMirror[];

int Gpu_GetControlMirrorByte(int arg0) {
    int ret;

    asm volatile(
        "lui %0,%%hi(g_GpuControlRegMirror)\n"
        "addu %0,%0,%1\n"
        "lbu %0,%%lo(g_GpuControlRegMirror)(%0)"
        : "=r"(ret)
        : "r"(arg0)
    );

    return ret;
}
