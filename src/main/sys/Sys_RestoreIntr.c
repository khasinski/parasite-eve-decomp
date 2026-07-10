/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef unsigned short u16;
typedef unsigned int u32;

typedef struct SysIntrState {
    u16 enabled;
    char pad2[0x30];
    u16 saved_mask;
    u32 saved_status;
    char hook[0x10];
} SysIntrState;

extern SysIntrState D_800945E4;
extern struct { char _[16]; } D_80095674_o __asm__("D_80095674");
extern struct { char _[16]; } D_80095678_o __asm__("D_80095678");

#define D_80095674 (*(u16 **)&D_80095674_o)
#define D_80095678 (*(u32 **)&D_80095678_o)

void HookEntryInt(void *hook);
void ExitCriticalSection(void);

void *Sys_RestoreIntr(void) {
    register volatile SysIntrState *state asm("$16");
    register u16 *mask_reg asm("$4");
    u32 *status_reg;
    register u16 saved_mask asm("$3");
    register u32 saved_status asm("$2");
    register u32 enabled asm("$2");

    state = &D_800945E4;
    if (state->enabled != 0) {
        return 0;
    }

    HookEntryInt(&state->hook);
    mask_reg = D_80095674;
    __asm__ volatile(".word 0x96030032" : "=r"(saved_mask) : "r"(state));
    enabled = 1;
    state->enabled = enabled;
    *mask_reg = saved_mask;
    status_reg = D_80095678;
    __asm__ volatile(".word 0x8E020034" : "=r"(saved_status) : "r"(state));
    __asm__ volatile("nop");
    *status_reg = saved_status;
    ExitCriticalSection();
    return state;
}
