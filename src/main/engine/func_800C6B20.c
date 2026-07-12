typedef signed short s16;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

int func_800C62DC(SVECTOR *from, void *to);

extern char *D_8009D254;

int func_800C6B20(void *arg0) {
    SVECTOR pos;
    int first;

    pos.vx = *(s16 *)(D_8009D254 + 0x2A);
    pos.vy = *(s16 *)(D_8009D254 + 0x2E);
    pos.vz = *(s16 *)(D_8009D254 + 0x32);

    first = func_800C62DC(&pos, arg0);
    return first | func_800C62DC(&pos, (char *)arg0 + 8);
}
