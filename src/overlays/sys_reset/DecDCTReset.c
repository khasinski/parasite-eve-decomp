/* GCC_VERSION: 2.8.1 */
#include "pe1/sys_reset.h"
#include "pe1/psyq_callbacks.h"
extern void func_8010C0FC(int mode);
void DecDCTReset(int mode) {
    if (mode == 0)
        ResetCallback();
    func_8010C0FC(mode);
}
