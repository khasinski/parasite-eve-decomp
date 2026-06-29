typedef unsigned char u8;

void Render_SetObjectAnim(void *arg0, int arg1, short arg2) {
    if (*(u8 *)(*(char **)arg0 + 2) == 2) {
        *(int *)((char *)arg0 + 0x24) = arg1;
        *(short *)((char *)arg0 + 0x28) = 3;
    } else {
        *(int *)((char *)arg0 + 0x24) = arg1;
        *(short *)((char *)arg0 + 0x28) = 1;
    }
    *(short *)((char *)arg0 + 0x2A) = arg2;
}
