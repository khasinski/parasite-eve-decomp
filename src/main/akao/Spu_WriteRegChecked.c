/* CC1_FLAGS: -g3 */


#include "include_asm.h"
typedef unsigned short u16;

extern volatile u16 g_SpuTransferAddr;
extern int _spu_mem_mode_plus;

int _spu_FsetRXXa(int arg0, int arg1);

asm(".globl Spu_WriteRegChecked");
asm("Spu_WriteRegChecked = Spu_WriteRegChecked");

int Spu_WriteRegChecked(int arg0) {
    int result;

    if ((unsigned int)(arg0 - 0x1010) <= 0x7EFE8) {
        g_SpuTransferAddr = _spu_FsetRXXa(-1, arg0);
        result = g_SpuTransferAddr << _spu_mem_mode_plus;
    } else {
        result = 0;
    }
    return result;
}
