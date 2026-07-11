/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

typedef int s32;

s32 DsRead_IsBusy(void);
CdlLOC *CdRom_GetCurrentPos(CdlLOC *dst);
s32 Render_BuildParticleFrame(s32 mode, CdlLOC *pos, s32 arg2, void *callback, s32 arg4);
s32 VSync(s32 mode);
s32 DsDataCallback(void *callback);
void CdRom_SetMode2Callback(unsigned char event);
void Render_StepParticleCallback(void);

extern char D_8009B6B0[];
extern s32 D_8009B6B8;
extern s32 D_8009B6C4;
extern s32 g_CdReadInProgress;

#define CD_READ_WORD(offset) (*(s32 *)(D_8009B6B0 + (offset)))

s32 CdRom_StartRead(CdlLOC *loc, s32 size, s32 dst, s32 mode) {
    CdlLOC start_loc;
    s32 ret;

    if (g_CdReadInProgress == 1) {
        return 0;
    }

    if (DsRead_IsBusy() != 0) {
        return 0;
    }

    CD_READ_WORD(-4) = 0x200;
    CD_READ_WORD(0) = dst;
    CD_READ_WORD(4) = size;

    if (loc == 0) {
        start_loc = *CdRom_GetCurrentPos(0);
    } else {
        start_loc = *loc;
    }
    mode |= 0x20;

    ret = Render_BuildParticleFrame(mode & 0xFF, &start_loc, 6, CdRom_SetMode2Callback, -1);
    if (ret == 0) {
        return 0;
    }

    D_8009B6C4 = VSync(-1);
    if (D_8009B6B8 & 1) {
        CD_READ_WORD(0x10) = DsDataCallback(Render_StepParticleCallback);
    }
    g_CdReadInProgress = 1;

    return ret;
}
