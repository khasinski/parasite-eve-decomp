typedef unsigned char u8;

void Render_ClearObjectAnim(void *arg0) {
    *(int *)((char *)arg0 + 0x24) = 0;
    if (*(u8 *)(*(char **)arg0 + 2) == 2) {
        *(short *)((char *)arg0 + 0x28) = 2;
    } else {
        *(short *)((char *)arg0 + 0x28) = 0;
    }
}
