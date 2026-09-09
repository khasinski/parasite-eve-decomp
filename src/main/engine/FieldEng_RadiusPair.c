int func_800C62DC(void *from, void *to);

int func_800C653C(void *arg0, char *arg1) {
    int first = func_800C62DC(arg0, arg1);

    return first | func_800C62DC(arg0, arg1 + 8);
}


typedef signed short s16;

int func_800C6584(s16 *a, int radiusA, s16 *b, int radiusB) {
    volatile int delta[3];
    int x = a[0] - b[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius;
    int radius_sq;
    delta[0] = x;
    z = a[2] - b[2];
    z_sq = z * z;
    radius = radiusA + radiusB;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}
