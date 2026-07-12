void func_801934A0(void* arg0, void* arg1, unsigned char* arg2) {
    unsigned int i;

    *(int*)arg2 = 0;

    for (i = 0; i < 8; i++) {
        *(arg2 + i + 0xB4) = 0;
    }
}
