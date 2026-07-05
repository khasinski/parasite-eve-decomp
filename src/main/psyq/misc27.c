#include "include_asm.h"

/* CC1_PROFILE: build0001 */

int LoadImage(int rect, int p);

int Gpu_LoadTimImage(int arg0) {
    int secondary;
    int image;
    int pixels;

    secondary = 0;
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        secondary = arg0 + 8;
        image = secondary + *(int *)(arg0 + 8);
    } else {
        image = arg0 + 8;
    }

    pixels = image + 0xC;
    LoadImage(image + 4, pixels);
    if (secondary != 0) {
        LoadImage(secondary + 4, secondary + 0xC);
    }
    return pixels;
}

int Str_GetTableEntryA(int arg0) {
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        return arg0 + 0xC;
    }
    return 0;
}

void *Widget_GetDataPtr(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return base + 4;
}

int Str_GetTableEntryB(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return (int)(base + 0xC);
}

int Str_GetTableEntryC(int arg0) {
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        return arg0 + 0x14;
    }
    return 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", exit);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", strcat);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", strncmp);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", strcpy);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", bzero);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", memcpy);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", memset);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", rand);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", srand);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc27", printf);

void __maspsx_include_asm_hack_Square_Vsprintf(void) {
    asm(
        ".text # maspsx-keep\n"
        ".include \"src/main/psyq/libc/Square_Vsprintf.s\" # maspsx-keep\n"
    );
}
