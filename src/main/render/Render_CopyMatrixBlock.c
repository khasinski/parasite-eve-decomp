typedef unsigned short u16;
typedef short s16;

void Render_CopyMatrixBlock(u16 *arg0, u16 *arg1, s16 count) {
    char unused[8];
    register u16 *src_cur asm("$7");
    register u16 *dst_cur asm("$8");
    int i;
    register char *src_tail asm("$4");
    register char *dst_tail asm("$5");
    int tmp;

    src_cur = arg0;
    dst_cur = arg1;
    i = 0;
    if (count > 0) {
        dst_tail = (char *)arg1 + 0x1C;
        src_tail = (char *)arg0 + 0x1C;
        do {
            *dst_cur = *src_cur;
            *(u16 *)(dst_tail - 0x1A) = *(u16 *)(src_tail - 0x1A);
            *(u16 *)(dst_tail - 0x18) = *(u16 *)(src_tail - 0x18);
            *(u16 *)(dst_tail - 0x16) = *(u16 *)(src_tail - 0x16);
            *(u16 *)(dst_tail - 0x14) = *(u16 *)(src_tail - 0x14);
            *(u16 *)(dst_tail - 0x12) = *(u16 *)(src_tail - 0x12);
            *(u16 *)(dst_tail - 0x10) = *(u16 *)(src_tail - 0x10);
            *(u16 *)(dst_tail - 0xE) = *(u16 *)(src_tail - 0xE);
            tmp = *(u16 *)(src_tail - 0xC);
            i++;
            *(u16 *)(dst_tail - 0xC) = tmp;
            tmp = *(int *)(src_tail - 8);
            src_cur += 0x10;
            *(int *)(dst_tail - 8) = tmp;
            tmp = *(int *)(src_tail - 4);
            dst_cur += 0x10;
            *(int *)(dst_tail - 4) = tmp;
            tmp = *(int *)src_tail;
            src_tail += 0x20;
            *(int *)dst_tail = tmp;
            dst_tail += 0x20;
        } while (i < count);
    }
}
