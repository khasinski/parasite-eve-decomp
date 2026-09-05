typedef signed short s16;

int func_800C6148(char *obj, s16 *pos, unsigned short radius) {
    volatile int delta[3];
    int x = *(s16 *)(obj + 0x2A) - pos[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius_sq;
    delta[0] = x;
    z = *(s16 *)(obj + 0x32) - pos[2];
    z_sq = z * z;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}
