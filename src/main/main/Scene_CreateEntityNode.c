/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 D_800A32D8[];
extern struct { char _[16]; } D_800A32D0_o __asm__("D_800A32D0");

int strcmp(char *lhs, char *rhs);
int firstfile(char *path, void *file);
void Sys_FirstFileHookCallback(void);

#define U32_AT(addr) (*(u32 *)(addr))
#define PTR_AT(ptr, off) (*(void **)((u8 *)(ptr) + (off)))
#define D_800A32D0 (*(void **)&D_800A32D0_o)

int Scene_CreateEntityNode(char *path, void *file) {
    register char *path_reg asm("$18");
    register void *file_reg asm("$19");
    char *src;
    u8 *dst;
    u8 *entry;
    u8 *end;
    register int found asm("$3");

    path_reg = path;
    file_reg = file;
    src = path_reg;
    dst = D_800A32D8;
    if ((s8)*src >= 0x3B) {
        do {
            *dst++ = *src++;
        } while ((s8)*src >= 0x3B);
    }
    *dst = 0;

    entry = (u8 *)U32_AT(0x150);
    end = entry + ((U32_AT(0x154) / 80) * 0x50);
    found = 0;
    while (entry < end) {
        if (PTR_AT(entry, 0) != 0 && strcmp(PTR_AT(entry, 0), (char *)D_800A32D8) == 0) {
            D_800A32D0 = PTR_AT(entry, 0x34);
            found = 1;
            break;
        }
        entry += 0x50;
    }

    if (found == 0) {
        return 0;
    }

    entry = (u8 *)U32_AT(0x150);
    end = entry + ((U32_AT(0x154) / 80) * 0x50);
    while (entry < end) {
        if (PTR_AT(entry, 0) != 0 && strcmp(PTR_AT(entry, 0), (char *)D_800A32D8) == 0) {
            PTR_AT(entry, 0x34) = Sys_FirstFileHookCallback;
            break;
        }
        entry += 0x50;
    }

    return firstfile(path_reg, file_reg);
}
