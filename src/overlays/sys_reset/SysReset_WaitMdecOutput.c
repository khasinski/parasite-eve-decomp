#include "common.h"
#include "pe1/sys_reset.h"

/* MASPSX_FLAGS: --la-call-delay */

#include "SysReset_WaitMdec.inc"

SYSRESET_WAIT_MDEC(func_8010C39C, D_8010DB30, 0x1000000, D_8010BD28)
