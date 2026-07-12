/* MASPSX_FLAGS: --expand-div */
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;

int rsin(int angle);
int rcos(int angle);

void func_800C4E50(char *data) {
    register char *data_s3 asm("$19");
    int angle;
    u32 i;
    int step;
    char *verts;

    data_s3 = data;
    asm("" : "=r"(data_s3) : "0"(data_s3));
    angle = 0;
    step = 0x1000 / *(u16 *)(data_s3 + 0xC);
    verts = *(char **)data_s3;

    for (i = 0; i < *(u16 *)(data_s3 + 0xC); i++, verts += 8) {
        *(s16 *)(verts + 0) = (rsin(angle) * *(u16 *)(data_s3 + 0x10)) >> 12;
        *(s16 *)(verts + 2) = (rcos(angle) * *(u16 *)(data_s3 + 0x10)) >> 12;
        *(s16 *)(verts + 4) = 0;
        angle += step;
    }

    angle = 0;
    for (i = 0; i < *(u16 *)(data_s3 + 0xC); i++, verts += 8) {
        *(s16 *)(verts + 0) = (rsin(angle) * *(u16 *)(data_s3 + 0xE)) >> 12;
        *(s16 *)(verts + 2) = (rcos(angle) * *(u16 *)(data_s3 + 0xE)) >> 12;
        *(s16 *)(verts + 4) = 0;
        angle += step;
    }
}
