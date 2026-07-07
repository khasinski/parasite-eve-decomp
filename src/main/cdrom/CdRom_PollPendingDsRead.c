/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --stack-return-delay */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 DsSync();
M2C_UNK CdRom_TryIssueCmd();
extern s32 g_CdDsReadIndex[];
#define g_CdDsReadIndex (g_CdDsReadIndex[0])
extern s32 g_CdPendingReadCount[];
#define g_CdPendingReadCount (g_CdPendingReadCount[0])

void CdRom_PollPendingDsRead(void);

void CdRom_PollPendingDsRead(void) {
    s32 temp_v0;
    void *temp_v1;

    temp_v0 = DsSync(0);
    if ((temp_v0 == 1) && (g_CdPendingReadCount > 0) && (DsSync(0) == temp_v0)) {
        temp_v1 = (g_CdDsReadIndex * 0x18) + ((u8 *)&g_CdPendingReadCount - 0xC8);
        if (M2C_FIELD(temp_v1, s32 *, 0) != 0) {
            CdRom_TryIssueCmd(M2C_FIELD(temp_v1, u8 *, 4), M2C_FIELD(temp_v1, s32 *, 0xC));
        }
    }
}
