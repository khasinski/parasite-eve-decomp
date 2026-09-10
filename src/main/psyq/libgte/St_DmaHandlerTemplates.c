#include "common.h"

/*
 * St_InstallDmaHandler compares the kernel exception handler with the first
 * six words and, when it matches, replaces it with the following six words.
 * These are instruction templates stored in CDROM.OBJ's executable section;
 * they are copied as data and are never entered at these addresses.
 */
const u32 St_DmaCompleteCallback[6] __attribute__((section(".text"))) = {
    0xAF410004, /* sw   $at, 0x04($k0) */
    0xAF420008, /* sw   $v0, 0x08($k0) */
    0xAF43000C, /* sw   $v1, 0x0C($k0) */
    0xAF5F007C, /* sw   $ra, 0x7C($k0) */
    0x40037000, /* mfc0 $v1, $14       */
    0x00000000, /* nop                  */
};

const u32 St_DmaPatchedCallback[6]
    __asm__("D_8007A1F8") __attribute__((section(".text"))) = {
        0xAF410004, /* sw   $at, 0x04($k0) */
        0xAF420008, /* sw   $v0, 0x08($k0) */
        0x40026800, /* mfc0 $v0, $13       */
        0xAF43000C, /* sw   $v1, 0x0C($k0) */
        0x40037000, /* mfc0 $v1, $14       */
        0xAF5F007C, /* sw   $ra, 0x7C($k0) */
};

const u32 St_DmaHandlerTemplatesEnd
    __asm__("D_8007A210") __attribute__((section(".text"))) = 0;
