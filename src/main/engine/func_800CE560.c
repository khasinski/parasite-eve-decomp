typedef unsigned short u16;

int func_800CE560(char *out, int stride, int count, int value) {
    int i;

    stride += 4;
    *(int *)(out + 8) = value;
    *(int *)(out + 0) = stride;
    *(int *)(out + 4) = count;

    for (i = 0; i < count; i++) {
        *(u16 *)(out + 0xC + (i * stride)) = 0;
    }

    return (stride * count) + 0xC;
}
