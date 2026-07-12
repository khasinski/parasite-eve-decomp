typedef signed short s16;

int func_800C6584(s16 *a, int radiusA, s16 *b, int radiusB) {
    int x = a[0] - b[0];
    int z = a[2] - b[2];
    int radius = radiusA + radiusB;

    return (x * x + z * z) < (radius * radius);
}
