#include "pe1/gte.h"
#include "pe1/gte_types.h"

int RotAverageNclip3(const GteShortVector *v0, const GteShortVector *v1,
    const GteShortVector *v2, u32 *sxy0, u32 * volatile sxy1,
    u32 * volatile sxy2, s32 * volatile p, s32 * volatile otz, u32 * volatile flag) {
    register u32 *flagPtr asm("$8");
    register u32 firstFlag asm("$9");
    register u32 *sxy1Ptr asm("$8");
    register u32 *sxy2Ptr asm("$9");
    register s32 *depthPtr asm("$10");
    s32 clip;
    register int zero asm("$0");
    register s32 *averagePtr asm("$9");
    register s32 average asm("$8");
    gte_lwc2_0_0(v0);
    gte_lwc2_1_4(v0);
    gte_lwc2_2_0(v1);
    gte_lwc2_3_4(v1);
    gte_lwc2_4_0(v2);
    gte_lwc2_5_4(v2);
    gte_cop2_hazard_slot();
    gte_rtpt_command();
    flagPtr = flag;
    gte_getflag(firstFlag);
    gte_cop2_hazard_slot();
    *flagPtr = firstFlag;
    gte_nclip_now();
    sxy1Ptr = sxy1;
    sxy2Ptr = sxy2;
    depthPtr = p;
    gte_getmac0(clip);
    gte_cop2_hazard_slot();
    if (clip > 0) {
        goto positive;
    }
    /* Bind a defined value to the architectural zero register.  This keeps
     * the original two-branch rejection path without CPU instruction asm. */
    asm volatile("" : "=r"(zero));
    if (zero == 0) {
        goto done;
    }
positive:
    gte_stsxy0(sxy0);
    gte_stsxy1(sxy1Ptr);
    gte_stsxy2(sxy2Ptr);
    gte_stir0(depthPtr);
    gte_cop2_hazard_slot();
    gte_avsz3_command();
    averagePtr = otz;
    gte_getotz(average);
    gte_cop2_hazard_slot();
    *averagePtr = average;
done:
    return clip;
}
/* Original trailing alignment words. */
static u32 tail_alignment_RotAverageNclip3[] __attribute__((section(".text"))) = { 0, 0 };
