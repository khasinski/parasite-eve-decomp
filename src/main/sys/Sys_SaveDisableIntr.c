#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef struct SysIntrState {
    u16 enabled;
    char pad2[0x30];
    u16 saved_mask;
    u32 saved_status;
    char hook[0x10];
} SysIntrState;

extern SysIntrState D_800945E4;
extern struct { char _[16]; } D_80095670_o __asm__("D_80095670");
extern struct { char _[16]; } D_80095674_o __asm__("D_80095674");
extern struct { char _[16]; } D_80095678_o __asm__("D_80095678");

#define D_80095670 (*(u16 **)&D_80095670_o)
#define D_80095674 (*(volatile u16 **)&D_80095674_o)
#define D_80095678 (*(u32 **)&D_80095678_o)

void EnterCriticalSection(void);
void ResetEntryInt(void);

void *Sys_SaveDisableIntr(void) {
    volatile SysIntrState *state;
    volatile u16 *mask_reg;
    register u16 *mask_shadow asm("$4");
    u32 *status_reg;
    register u16 saved_mask asm("$3");
    register u32 saved_status asm("$3");
    u32 status;
    u32 mask;
    state = &D_800945E4;
    if (state->enabled == 0) {
        return 0;
    }

    EnterCriticalSection();
    mask_reg = D_80095674;
    __asm__ volatile(
        "lui $4, %%hi(D_80095678)\n"
        "lw $4, %%lo(D_80095678)($4)"
        : "=r"(status_reg));
    saved_mask = *mask_reg;
    __asm__ volatile("nop");
    __asm__ volatile(".word 0xA6030032" : : "r"(saved_mask), "r"(state));
    saved_status = *status_reg;
    mask_shadow = D_80095670;
    __asm__ volatile(".word 0xAE030034" : : "r"(saved_status), "r"(state));
    __asm__ volatile(
        ".word 0xA4400000\n"
        ".word 0x94420000\n"
        "nop\n"
        ".word 0xA4820000"
        :
        : "r"(mask_reg), "r"(mask_shadow));
    __asm__ volatile(
        "lui $4, %%hi(D_80095678)\n"
        "lw $4, %%lo(D_80095678)($4)"
        : "=r"(status_reg));
    status = *status_reg;
    mask = 0x77777777;
    status &= mask;
    *status_reg = status;
    ResetEntryInt();
    state->enabled = 0;
    return (void *)state;
}
