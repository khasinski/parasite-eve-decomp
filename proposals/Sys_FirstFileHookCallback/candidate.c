/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"

typedef int (*FirstFileCallback)(int *state, void *arg1, void *arg2);
typedef struct FirstFileDevice {
    char *name;
    unsigned char reserved04[0x30];
    FirstFileCallback firstFile;
    unsigned char reserved38[0x18];
} FirstFileDevice;
typedef char FirstFileDeviceSizeCheck[(sizeof(FirstFileDevice) == 0x50) ? 1 : -1];

extern FirstFileCallback D_800A32D0;
extern char D_800A32D8[];
extern int strcmp(const char *, const char *);

int Sys_FirstFileHookCallback(int *state, void *arg1, void *arg2) {
    FirstFileDevice *entry;
    FirstFileDevice *end;
    FirstFileCallback original;
    u32 bytes;

    if (*state == 0) *state = 1;
    bytes = *(volatile u32 *)0x154;
    entry = *(FirstFileDevice *volatile *)0x150;
    original = D_800A32D0;
    end = entry + bytes / sizeof(FirstFileDevice);
    while (entry < end) {
        if (entry->name && strcmp(entry->name, D_800A32D8) == 0) {
            entry->firstFile = original;
            break;
        }
        entry++;
    }
    return D_800A32D0(state, arg1, arg2);
}
