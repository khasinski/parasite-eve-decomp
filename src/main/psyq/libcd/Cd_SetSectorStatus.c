#include "common.h"

typedef struct CdSectorDataWindow {
    int status;
    u8 pad_B264[0x4D9C];
} CdSectorDataWindow;

register CdSectorDataWindow *g_CdSectorDataWindow asm("$1");

void func_8007C130(int arg0) {
    g_CdSectorDataWindow = (CdSectorDataWindow *)0x800A0000;
    g_CdSectorDataWindow[-1].status = arg0;
}
