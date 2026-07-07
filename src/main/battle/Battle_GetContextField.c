typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

extern void **g_PlayerEntity;

int Battle_GetContextField(int arg0) {
    u8 *ctx;
    int ret;
    u32 word;

    arg0 = (u8)arg0;
    ctx = g_PlayerEntity[0];
    ret = -1000;

    switch (arg0) {
    case 0:
        ret = *(int *)(ctx + 0x0);
        break;
    case 1:
        ret = *(s16 *)(ctx + 0x4);
        break;
    case 2:
        ret = *(u16 *)(ctx + 0x6);
        break;
    case 3:
        ret = *(int *)(ctx + 0x8);
        break;
    case 4:
        ret = *(s16 *)(ctx + 0xC);
        break;
    case 6:
        ret = *(u16 *)(ctx + 0x10);
        break;
    case 7:
        ret = *(u8 *)(ctx + 0x12);
        break;
    case 10:
        ret = *(s16 *)(ctx + 0x1C);
        break;
    case 11:
        ret = *(u16 *)(ctx + 0x1E);
        break;
    case 12:
        ret = *(u16 *)(ctx + 0x20);
        break;
    case 13:
        ret = *(int *)(ctx + 0x28);
        break;
    case 14:
        ret = *(u16 *)(ctx + 0x22);
        break;
    case 20:
        word = *(u32 *)(ctx + 0x4C);
        ret = word >> 9;
        ret &= 1;
        break;
    case 21:
        word = *(u32 *)(ctx + 0x4C);
        ret = word >> 6;
        ret &= 3;
        break;
    case 22:
        word = *(u32 *)(ctx + 0x4C);
        ret = word >> 29;
        ret &= 1;
        break;
    }

    return ret;
}
