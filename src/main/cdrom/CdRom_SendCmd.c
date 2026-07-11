/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef int s32;

void CD_flush(void);
s32 CD_cw(s32 cmd, void *params, s32 arg2, s32 arg3);
void Util_Copy4(u8 *dst, u8 *src);

extern u8 D_8009B558[];
extern s32 g_CdRomCmdTimeout __asm__("D_8009B598");
extern s32 D_8009B59C;
extern s32 g_CdRomCmdLongTimeoutTable[];

s32 CdRom_SendCmd(s32 cmd, u8 *params) {
    s32 timeout;

    CD_flush();

    D_8009B558[0] = cmd;
    if (params != 0) {
        Util_Copy4(&D_8009B558[1], params);
        *(u8 **)&D_8009B558[8] = &D_8009B558[1];
    } else {
        *(u8 **)&D_8009B558[8] = 0;
    }

    timeout = 0x1E;
    if (g_CdRomCmdLongTimeoutTable[D_8009B558[0]] != 0) {
        timeout = 0x3C0;
    }
    g_CdRomCmdTimeout = timeout;
    D_8009B59C = 0;

    if (D_8009B558[0] == 7) {
        if (D_8009B558[0x33] == 1) {
            D_8009B558[0] = D_8009B558[0x33];
            *(s32 *)&D_8009B558[8] = 0;
        }
    } else if (D_8009B558[0] == 8) {
        if (D_8009B558[0x33] != 1) {
            D_8009B558[0] = 1;
            *(s32 *)&D_8009B558[8] = 0;
        }
    }

    if (CD_cw(D_8009B558[0], *(void **)&D_8009B558[8], 0, 1) == 0) {
        D_8009B558[0x28] = D_8009B558[0];
        return 1;
    }

    D_8009B59C = 0;
    g_CdRomCmdTimeout = 0;
    return 0;
}
