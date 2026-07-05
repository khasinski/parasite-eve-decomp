asm("SetDrawMask = SetDrawMask");

void Gpu_InitDrawMaskSetPacket(void *arg0, int arg1);

void SetDrawMask(void *arg0, int arg1, int arg2) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) =
        (arg1 ? 0xE6000002 : 0xE6000000) | (arg2 != 0);
    *(int *)((char *)arg0 + 8) = 0;
}

void Gpu_InitDrawMaskSetPacket(void *arg0, int arg1) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) = arg1 ? 0xE6000001 : 0xE6000000;
    *(int *)((char *)arg0 + 8) = 0;
}
