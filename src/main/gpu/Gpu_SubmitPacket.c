void Gpu_SwapDisplayBuffers(int arg0, int arg1, int arg2, int arg3);

void Gpu_SubmitPacket(int arg0, int arg1, int arg2) {
    Gpu_SwapDisplayBuffers(arg0, arg1, 0, arg2);
}
