/* MASPSX_FLAGS: -G8 */


#include "include_asm.h"
void CdRom_ReadProgressCallback(void);
void CdRom_InitAsyncRead(void (*arg0)(void), int arg1);

asm(".globl func_80080F64");
asm("func_80080F64 = CdRom_SetMode2Callback");

void CdRom_SetMode2Callback(unsigned char arg0) {
    if (arg0 == 2) {
        CdRom_InitAsyncRead(CdRom_ReadProgressCallback, -1);
    }
}
