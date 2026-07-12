typedef unsigned short u16;
typedef signed short s16;

int rsin(int angle);
int rcos(int angle);

void func_800C4E50(char *data) {
    int angle = 0;
    int i;
    int step = 0x1000 / *(u16 *)(data + 0xC);
    char *verts = *(char **)data;

    for (i = 0; i < *(u16 *)(data + 0xC); i++, verts += 8) {
        *(s16 *)(verts + 0) = (rsin(angle) * *(u16 *)(data + 0x10)) >> 12;
        *(s16 *)(verts + 4) = 0;
        *(s16 *)(verts + 2) = (rcos(angle) * *(u16 *)(data + 0x10)) >> 12;
        angle += step;
    }

    angle = 0;
    for (i = 0; i < *(u16 *)(data + 0xC); i++, verts += 8) {
        *(s16 *)(verts + 0) = (rsin(angle) * *(u16 *)(data + 0xE)) >> 12;
        *(s16 *)(verts + 4) = 0;
        *(s16 *)(verts + 2) = (rcos(angle) * *(u16 *)(data + 0xE)) >> 12;
        angle += step;
    }
}
