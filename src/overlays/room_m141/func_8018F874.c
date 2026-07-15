typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern s16 D_800942EC;

void *func_800C2B50(void);
int func_800C6B90(void *arg0, s16 arg1);

void func_8018F874(void *arg0, u8 *arg1, char *arg2) {
    s16 pos[3];
    char *entity;
    s16 value;

    entity = func_800C2B50();

    if (*(s16 *)(arg1 + 2) >= 9) {
        value = *(s16 *)(arg2 + 0x122);
        if (value >= 0x21) {
            *(s16 *)(arg2 + 0x122) = value - 0x20;
        }
    }

    *(s16 *)(arg2 + 0x120) = *(u16 *)(arg2 + 0x120) + ((s16)*(u16 *)(arg2 + 0x124) >> 4);
    *(s16 *)(arg2 + 0x124) = *(u16 *)(arg2 + 0x124) + 0x320;

    if (*(s16 *)(arg1 + 2) == 0x10) {
        arg1[1] = 2;
    }

    pos[0] = *(int *)(entity + 0x18);
    pos[1] = D_800942EC;
    pos[2] = *(int *)(entity + 0x20);
    if (func_800C6B90(pos, *(s16 *)(entity + 0x2E)) != 0) {
        *(s16 *)(entity + 0x2C) = 1;
    }
}
