typedef signed short s16;

extern char *D_8009D254;

int func_800C6B90(s16 *pos, int extraRadius) {
    int x = *(s16 *)(D_8009D254 + 0x2A) - pos[0];
    int z = *(s16 *)(D_8009D254 + 0x32) - pos[2];
    int radius = *(s16 *)(D_8009D254 + 0x224) + extraRadius;

    return (x * x + z * z) < (radius * radius);
}
