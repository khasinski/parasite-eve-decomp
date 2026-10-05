typedef signed short s16;

void func_8019001C(void* arg0, void* arg1, unsigned char* arg2) {
    s16 value = 0x80;

    arg2[2] = 0;
    *(s16*)(arg2 + 4) = value;
}
