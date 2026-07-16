typedef signed short s16;
typedef int s32;

char *func_800C2B50(void);

void func_80191250(void *arg0, void *arg1, char *fx) {
    char *owner;
    register char *half_ptr asm("$4");
    register char *word_ptr asm("$5");
    s32 i;
    register char *wide_ptr asm("$7");
    s32 one;
    s32 eighty;

    owner = func_800C2B50();
    i = 0;
    one = 1;
    eighty = 0x80;
    word_ptr = fx;
    wide_ptr = word_ptr;
    *(s16 *)(fx + 0xEA) = 0x80;
    *(s32 *)(fx + 0xE0) = 0xFFEC0000;
    *(s32 *)(fx + 0xE4) = *(s32 *)(owner + 0x58) << 8;
    half_ptr = word_ptr;

    do {
        *(s16 *)(half_ptr + 0x88) = 0;
        *(s32 *)(word_ptr + 0xA8) = 0;
        word_ptr += 4;
        *(s32 *)(wide_ptr + 0xC) = *(s16 *)(fx + 2) << 16;
        wide_ptr += 0x10;
        *(fx + i + 0xC8) = one;
        i++;
        *(s16 *)(half_ptr + 0xD0) = eighty;
        half_ptr += 2;
    } while ((unsigned int)i < 8);
}
