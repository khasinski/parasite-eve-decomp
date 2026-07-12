typedef signed short s16;

int func_800C6148(char *obj, s16 *pos, unsigned short radius) {
    int x = *(s16 *)(obj + 0x2A) - pos[0];
    int z = *(s16 *)(obj + 0x32) - pos[2];

    return (x * x + z * z) < (radius * radius);
}
