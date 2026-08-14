typedef unsigned int u32;

typedef struct SysFileEntry {
    /* 0x00 */ char *name;
    /* 0x04 */ unsigned char pad_04[0x30];
    /* 0x34 */ int hook;
    /* 0x38 */ unsigned char pad_38[0x18];
} SysFileEntry; /* size 0x50 */

extern int D_800A32D0;
extern char D_800A32D8[];

int strcmp(const char *s1, const char *s2);

int Sys_FirstFileHookCallback(int *arg0, int arg1, int arg2) {
    SysFileEntry *entry;
    SysFileEntry *end0;
    SysFileEntry *end;
    int saved;

    if (*arg0 == 0) {
        *arg0 = 1;
    }

    entry = *(SysFileEntry **)0x150;
    saved = D_800A32D0;
    end0 = entry + (*(u32 *)0x154 / 0x50);
    if (entry < end0) {
        end = end0;
        do {
            if (entry->name != 0 && strcmp(entry->name, D_800A32D8) == 0) {
                entry->hook = saved;
                break;
            }
            entry++;
        } while (entry < end);
    }

    return ((int (*)(int *, int, int))D_800A32D0)(arg0, arg1, arg2);
}
