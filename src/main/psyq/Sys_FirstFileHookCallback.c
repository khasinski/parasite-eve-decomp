/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;

extern u8 D_800A32D8[];
extern struct { char _[16]; } D_800A32D0_o __asm__("D_800A32D0");

int strcmp(char *lhs, char *rhs);

#define U32_AT(addr) (*(u32 *)(addr))
#define PTR_AT(ptr, off) (*(void **)((u8 *)(ptr) + (off)))
#define D_800A32D0 (*(int (**)(int *result, void *file, void *extra))&D_800A32D0_o)

int Sys_FirstFileHookCallback(int *result, void *file, void *extra) {
    register int *result_reg asm("$18");
    register void *file_reg asm("$20");
    register void *extra_reg asm("$21");
    register void *saved_callback asm("$19");
    u8 *entry;
    u8 *end;

    result_reg = result;
    file_reg = file;
    extra_reg = extra;

    if (*result_reg == 0) {
        *result_reg = 1;
    }

    entry = (u8 *)U32_AT(0x150);
    end = entry + ((U32_AT(0x154) / 80) * 0x50);
    saved_callback = D_800A32D0;
    while (entry < end) {
        if (PTR_AT(entry, 0) != 0 && strcmp(PTR_AT(entry, 0), (char *)D_800A32D8) == 0) {
            PTR_AT(entry, 0x34) = saved_callback;
            break;
        }
        entry += 0x50;
    }

    return D_800A32D0(result_reg, file_reg, extra_reg);
}
