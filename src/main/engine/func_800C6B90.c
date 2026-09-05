typedef signed short s16;
extern char *D_8009D254;

int func_800C6B90(s16 *pos, int extraRadius) {
    volatile int delta[3];
    volatile s16 local[4];
    char *entity = D_8009D254;
    int x = *(s16 *)(entity + 0x2A);
    int radius = *(s16 *)(entity + 0x224);
    int z;
    int x_sq;
    int z_sq;
    int radius_sq;
    local[0] = x;
    {
        int y;
        y = *(s16 *)(entity + 0x2E);
        local[1] = y;
    }
    z = *(s16 *)(entity + 0x32);
    local[2] = z;
    x -= pos[0];
    x_sq = x * x;
    delta[0] = x;
    z -= pos[2];
    z_sq = z * z;
    radius += extraRadius;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}
