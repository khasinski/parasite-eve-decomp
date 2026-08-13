
typedef signed short s16;

typedef struct ArcPointComponent {
    s16 value;
    s16 reserved02;
} ArcPointComponent;

typedef struct AlternatingArcPoints {
    ArcPointComponent component[8];
} AlternatingArcPoints;

int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);
int func_800C6B20(void *arg0);

static inline int div4096(int value) {
    if (value < 0) {
        value += 0xFFF;
    }
    return value >> 12;
}

int func_800CEB8C(s16 *a, s16 *b, int radius) {
    /* Match note: target reuses $s0 for the angle and then the x offset. */
    register int angle asm("$16");
    AlternatingArcPoints points;
    int dx;
    int dz;
    int x;
    register int z asm("$6");

    dx = b[0] - a[0];
    dz = b[2] - a[2];
    angle = -Gte_Atan2(dz, dx);
    x = div4096(rsin(angle) * radius);
    z = div4096(rcos(angle) * radius);

    points.component[0].value = a[0] - x;
    points.component[1].value = a[2] - z;
    points.component[2].value = a[0] + x;
    points.component[3].value = a[2] + z;
    points.component[4].value = b[0] - x;
    points.component[5].value = b[2] - z;
    points.component[6].value = b[0] + x;
    points.component[7].value = b[2] + z;

    return func_800C6B20(&points);
}
