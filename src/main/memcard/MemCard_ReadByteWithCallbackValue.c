#include "common.h"

/* MASPSX_FLAGS: --stack-return-delay */

extern s32 (*D_8009B73C)(s32 value);
extern s32 D_8009B7A4;

void MemCard_ReadByte(s32 value, s32 command);

void MemCard_ReadByteWithCallbackValue(s32 value)
{
    D_8009B7A4 = D_8009B73C(value);
    MemCard_ReadByte(value, -2);
}
