asm(".globl Gpu_InitDrawMaskSetPacket");
asm("Gpu_InitDrawMaskSetPacket = func_80075C6C");

void Gpu_InitDrawMaskSetPacket(void *arg0, int arg1) __asm__("func_80075C6C");

void Gpu_InitDrawMaskSetPacket(void *arg0, int arg1) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) = arg1 ? 0xE6000001 : 0xE6000000;
    *(int *)((char *)arg0 + 8) = 0;
}
