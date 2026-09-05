typedef signed short s16;
typedef struct { s16 vx, vy, vz, pad; } SVECTOR;
int func_800C62DC(SVECTOR *from, void *to);
extern char *D_8009D254;

int func_800C6B20(void *arg0) {
    SVECTOR pos;
    char *entity = D_8009D254;
    int first;
    register int value asm("$2");
    SVECTOR *ptr;
    value = *(s16 *)(entity + 0x2A);
    ptr = &pos;
    pos.vx = value;
    value = *(s16 *)(entity + 0x2E);
    pos.vy = value;
    value = *(s16 *)(entity + 0x32);
    pos.vz = value;
    first = func_800C62DC(ptr, arg0);
    return first | func_800C62DC(ptr, (char *)arg0 + 8);
}
