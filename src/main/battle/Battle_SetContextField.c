/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

typedef unsigned char u8;
typedef short s16;

extern void **g_PlayerEntity;
extern s16 D_800942EC;

void Battle_SetContextField(int arg0, int arg1) {
    u8 *ctx;

    arg0 = (u8)arg0;
    ctx = g_PlayerEntity[0];

    switch (arg0) {
    case 0:
        *(int *)(ctx + 0x0) = arg1;
        break;
    case 1:
        *(s16 *)(ctx + 0x4) = arg1;
        break;
    case 2:
        *(s16 *)(ctx + 0x6) = arg1;
        break;
    case 3:
        *(int *)(ctx + 0x8) = arg1;
        break;
    case 4:
        *(s16 *)(ctx + 0xC) = arg1;
        break;
    case 5:
        *(s16 *)(ctx + 0xE) = arg1;
        break;
    case 6:
        *(s16 *)(ctx + 0x10) = arg1;
        break;
    case 10:
        *(s16 *)(ctx + 0x1C) = arg1;
        break;
    case 11:
        *(s16 *)(ctx + 0x1E) = arg1;
        break;
    case 12:
        *(s16 *)(ctx + 0x20) = arg1;
        break;
    case 14:
        *(s16 *)(ctx + 0x22) = arg1;
        break;
    case 18:
        *(s16 *)(ctx + 0x26) = arg1;
        break;
    case 30:
        *(s16 *)(ctx + 0x50) = arg1;
        break;
    case 31:
        *(u8 *)(ctx + 0x56) = arg1;
        break;
    case 32:
        *(u8 *)(ctx + 0x57) = arg1;
        break;
    case 33:
        *(s16 *)(ctx + 0x58) = arg1;
        break;
    case 34:
        *(u8 *)(ctx + 0x5E) = arg1;
        break;
    case 255:
        D_800942EC = arg1;
        break;
    }
}
