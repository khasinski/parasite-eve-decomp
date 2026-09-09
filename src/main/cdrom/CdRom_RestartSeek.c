/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fno-expensive-optimizations -fcall-used-$1 */
#include "pe1/psyq_cd.h"

extern int D_8009B6EC;
extern CdlLOC *CdRom_GetCurrentPosPtr(void);
extern int CdPosToInt(CdlLOC *);
extern int CdRom_GetCmdMode(void);
extern int func_8007FC44(void);
extern void func_8008227C(void);
extern int Render_BuildParticleFrame(int, CdlLOC *, int, void (*)(void), int);

int CdRom_RestartSeek(void) {
    int mode;
    CdlLOC *position;
    int command;
    int argumentMode;
    register int limit asm("$3");

    DsSyncCallback(0);
    D_8009B6EC = CdPosToInt(CdRom_GetCurrentPosPtr());
    mode = CdRom_GetCmdMode() & 0xFF;
    position = CdRom_GetCurrentPosPtr();
    command = func_8007FC44();
    argumentMode = mode;
    limit = -1;
    return Render_BuildParticleFrame(argumentMode, position,
        (unsigned char)command, func_8008227C, limit);
}
