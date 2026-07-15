#include "include_asm.h"

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

#define PSYQ_BIOS_THUNK(name, vector, call) \
    asm( \
        ".text\n" \
        ".set noreorder\n" \
        ".globl " #name "\n" \
        ".ent " #name "\n" \
        #name ":\n" \
        "addiu $10,$0," #vector "\n" \
        "jr $10\n" \
        "addiu $9,$0," #call "\n" \
        ".end " #name "\n" \
        "nop\n" \
        ".set reorder\n" \
    )

/* Match note: these PSY-Q libc entry points are BIOS trampolines with custom
 * ABI/delay-slot ownership. The trailing nop is inter-function padding. */
PSYQ_BIOS_THUNK(exit, 0xB0, 0x38);
PSYQ_BIOS_THUNK(strcat, 0xA0, 0x15);
PSYQ_BIOS_THUNK(strncmp, 0xA0, 0x18);
PSYQ_BIOS_THUNK(strcpy, 0xA0, 0x19);
PSYQ_BIOS_THUNK(bzero, 0xA0, 0x28);
PSYQ_BIOS_THUNK(memcpy, 0xA0, 0x2A);
PSYQ_BIOS_THUNK(memset, 0xA0, 0x2B);
PSYQ_BIOS_THUNK(rand, 0xA0, 0x2F);
PSYQ_BIOS_THUNK(srand, 0xA0, 0x30);
PSYQ_BIOS_THUNK(printf, 0xA0, 0x3F);

void __maspsx_include_asm_hack_Square_Vsprintf(void) {
    asm(
        ".text # maspsx-keep\n"
        ".include \"src/main/psyq/libc/Square_Vsprintf.inc\" # maspsx-keep\n"
    );
}
